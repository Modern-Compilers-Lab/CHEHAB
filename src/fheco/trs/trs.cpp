#include "fheco/dsl/compiler.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/fold_op_gen_matcher.hpp"
#include "fheco/trs/term_matcher.hpp"
#include "fheco/trs/trs.hpp"
#ifdef FHECO_LOGGING
#include "fheco/util/expr_printer.hpp"
#include <iostream>
#endif
#include <stack>
#include <stdexcept>
#include <unordered_map> 
#include <utility>
#include <iostream>
using namespace std;

namespace fheco::trs
{

bool TRS::run(RewriteHeuristic heuristic, int64_t max_iter, bool rewrite_created_sub_terms, bool global_analysis)
{
  bool order_operands_enabled = Compiler::order_operands_enabled();
  Compiler::disable_order_operands();
  #ifdef FHECO_LOGGING
  util::ExprPrinter expr_printer{func_}; 
  clog << "\ninitial IR, ";
  expr_printer.print_expand_outputs_str_expr(clog);
  #endif
  int64_t iter = max_iter;
  bool did_rewrite = false;
  switch (heuristic)
  {
    case RewriteHeuristic::bottom_up:
      for (auto id : func_->get_top_sorted_terms_ids()){
        did_rewrite = rewrite_term(id, RewriteHeuristic::bottom_up, iter, rewrite_created_sub_terms, global_analysis);
      }
      break;

    case RewriteHeuristic::top_down:
    {
      const auto &sorted_terms_ids = func_->get_top_sorted_terms_ids();
      for (auto id_it = sorted_terms_ids.crbegin(); id_it != sorted_terms_ids.crend(); ++id_it)
        did_rewrite = rewrite_term(*id_it, RewriteHeuristic::top_down, iter, rewrite_created_sub_terms, global_analysis);

      break;
    }
    default:
      throw logic_error("unhandled RewriteHeuristic");
  }
  #ifdef FHECO_LOGGING
  clog << "\nfinal IR, ";
  expr_printer.print_expand_outputs_str_expr(clog);
  clog << '\n';
  clog << "performed " << max_iter - iter << " rewrite attempts\n";
  #endif
  if (order_operands_enabled)
    Compiler::enable_order_operands();
    return did_rewrite;
}

bool TRS::apply_rule(ir::Term *term, const Rule &rule)
{
  bool order_operands_enabled = Compiler::order_operands_enabled();
  Compiler::disable_order_operands();
  #ifdef FHECO_LOGGING
    util::ExprPrinter expr_printer{func_};
    clog << "applying rule \"" << util::ExprPrinter::make_rule_str_repr(rule) << "\" on term \""
        << expr_printer.expand_term_str_expr(term) << "\"\n";
  #endif
  Substitution subst;
  bool global_analysis = false;
  double rel_cost = 0;
  ir::Term::PtrSet to_delete;
  bool matched = match(rule.lhs(), term, subst, global_analysis, rel_cost, to_delete);
  if (!matched)
  {
    #ifdef FHECO_LOGGING
        clog << "matching failed\n";
    #endif
    return false;
  }
  #ifdef FHECO_LOGGING
    clog << "unified with substitution:\n";
    clog << "σ = ";
    pprint_substitution(func_, subst, clog);
    clog << '\n';
  #endif
    if (!rule.check_cond(subst))
    {
  #ifdef FHECO_LOGGING
      clog << "condition not satisfied\n";
  #endif
    if (order_operands_enabled)
      Compiler::enable_order_operands();
    return false;
  }
  vector<size_t> created_terms_ids;
  auto equiv_term = construct_term(rule.get_rhs(subst), subst, to_delete, global_analysis, rel_cost, created_terms_ids);
  #ifdef FHECO_LOGGING
    clog << "replace with \"" << expr_printer.expand_term_str_expr(equiv_term) << "\"\n";
  #endif
  func_->replace_term_with(term, equiv_term);
  if (order_operands_enabled)
    Compiler::enable_order_operands();
  return true;
}

bool TRS::rewrite_term(
  size_t id, RewriteHeuristic heuristic, int64_t &max_iter, bool rewrite_created_sub_terms, bool global_analysis)
{
  #ifdef FHECO_LOGGING
    util::ExprPrinter expr_printer{func_};
  #endif
  bool did_rewrite = false;
  stack<size_t> call_stack;
  call_stack.push(id);
  while (max_iter > 0 && !call_stack.empty())
  {
    auto top_term_id = call_stack.top();
    call_stack.pop();

    auto top_term = func_->data_flow().get_term(top_term_id);
    if (!top_term)
      continue;

    if (top_term->is_leaf())
      continue;

    --max_iter;

    #ifdef FHECO_LOGGING
        clog << "\nrewriting term \"" << expr_printer.expand_term_str_expr(top_term) << "\"\n";
    #endif
    for (const auto &rule : ruleset_.pick_rules(top_term->op_code().type()))
    {
      #ifdef FHECO_LOGGING
            clog << "trying rule \"" << util::ExprPrinter::make_rule_str_repr(rule) << "\", ";
      #endif
      Substitution subst;
      double rel_cost = 0;
      ir::Term::PtrSet to_delete;
      bool matched = match(rule.lhs(), top_term, subst, global_analysis, rel_cost, to_delete);

      if (!matched)
      {
        #ifdef FHECO_LOGGING
                clog << "matching failed\n";
        #endif
        continue;
      }
      #ifdef FHECO_LOGGING
            clog << "unified with substitution:\n";
            clog << "σ = ";
            pprint_substitution(func_, subst, clog);
            clog << '\n';
      #endif

      if (!rule.check_cond(subst))
      {
        #ifdef FHECO_LOGGING
                clog << "condition not satisfied\n";
        #endif
        continue;
      }

      vector<size_t> created_terms_ids;
      auto equiv_term =
        construct_term(rule.get_rhs(subst), subst, to_delete, global_analysis, rel_cost, created_terms_ids);
      #ifdef FHECO_LOGGING
            if (global_analysis)
              clog << "constructed rhs, rel_cost=" << rel_cost << '\n';
      #endif

      if (!global_analysis || rel_cost <= 0)
      {
        #ifdef FHECO_LOGGING
            clog << "replace with \"" << expr_printer.expand_term_str_expr(equiv_term) << "\"\n";
        #endif
        func_->replace_term_with(top_term, equiv_term);
        did_rewrite = true;

        if (rewrite_created_sub_terms)
        {
          switch (heuristic)
          {
            case RewriteHeuristic::bottom_up:
              for (auto created_term_id_it = created_terms_ids.crbegin(); created_term_id_it != created_terms_ids.crend();
                  ++created_term_id_it)
                call_stack.push(*created_term_id_it);
              break;

            case RewriteHeuristic::top_down:
              for (auto created_term_id : created_terms_ids)
                call_stack.push(created_term_id);
              break;

            default:
              throw logic_error("unhandled RewriteHeuristic");
          }
        }
        else
        {
          if (created_terms_ids.size() && created_terms_ids.back() == equiv_term->id())
            call_stack.push(equiv_term->id());
        }
        break;
      }
      else if (func_->data_flow().can_delete(equiv_term))
      #ifdef FHECO_LOGGING
              clog << "delete constructed equiv_term \"" << expr_printer.expand_term_str_expr(equiv_term) << "\"\n";
      #endif
      func_->delete_term_cascade(equiv_term);
    }
  }
  return did_rewrite;
}

bool TRS::match(
  const TermMatcher &term_matcher, ir::Term *term, Substitution &subst, bool global_analysis, double &rel_cost,
  ir::Term::PtrSet &to_delete) const
{
  struct Call
  {
    ir::Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  ir::Term::PtrSet visited_terms;
  ir::Term::PtrSet subst_terms;
  vector<ir::Term *> sorted_terms;
  stack<TermMatcher::RefWrapp> term_matchers;

  term_matchers.push(term_matcher);
  call_stack.push(Call{term, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    
    /*if (global_analysis && top_call.children_processed_)
    {
      visited_terms.insert(top_term);
      sorted_terms.push_back(top_term);
      continue;
    }*/

    if (term_matchers.empty())
    {
      if (!call_stack.empty())
        throw logic_error("term_matchers empty and call_stack not empty in match");

      break;
    }
    const TermMatcher &top_term_matcher = term_matchers.top();
    term_matchers.pop();
    if (top_term_matcher.is_variable())
    {
      if (top_term_matcher.type() == TermMatcherType::const_)
      {
        if (!func_->data_flow().is_const(top_term))
          return false;
      }
      else
      {
        if (top_term_matcher.type() != top_term->type())
          return false;
      }
      if (!subst.insert(top_term_matcher, top_term))
        return false;

      if (global_analysis)
        subst_terms.insert(top_term);
    }
    else
    {
      if (top_term_matcher.val())
      {
        auto const_val = func_->data_flow().get_const_val(top_term);
        if (!const_val)
          return false;

        if (*top_term_matcher.val() != *const_val)
          return false;
      }
      else
      {
        if (top_term_matcher.op_code().type() != top_term->op_code().type())
          return false;

        for (size_t i = 0; i < top_term_matcher.op_code().generators().size(); ++i)
        {
          const auto &op_gen_matcher = top_term_matcher.op_code().generators()[i];
          if (op_gen_matcher.is_variable())
          {
            if (!subst.insert(op_gen_matcher, top_term->op_code().generators()[i]))
              return false;
          }
          else if (op_gen_matcher.val())
          {
            if (*op_gen_matcher.val() != top_term->op_code().generators()[i])
              return false;
          }
          else
            throw invalid_argument("failed to match an operation op_gen_matcher");
        }
        if (top_term_matcher.type() != top_term->type())
          return false;

        /*if (global_analysis)
        {
          if (auto it = visited_terms.find(top_term); it == visited_terms.end())
            call_stack.push(Call{top_term, true});
        }*/
        for (size_t i = 0; i < top_term_matcher.operands().size(); ++i)
        {
          term_matchers.push(top_term_matcher.operands()[i]);
          call_stack.push(Call{top_term->operands()[i], false});
        }
      }
    }
  }
  /*if (global_analysis)
  {
    to_delete.insert(term);
    rel_cost = -ir::static_eval_op(func_, term->op_code(), term->operands());
    sorted_terms.pop_back();
    for (auto sorted_term_it = sorted_terms.crbegin(); sorted_term_it != sorted_terms.crend(); ++sorted_term_it)
    {
      auto sorted_term = *sorted_term_it;
      if (subst_terms.find(sorted_term) != subst_terms.end())
        continue;

      auto [to_delete_it, inserted] = to_delete.insert(sorted_term);
      for (auto parent : sorted_term->parents())
      {
        if (to_delete.find(parent) == to_delete.end())
        {
          to_delete.erase(to_delete_it);
          to_delete_it = to_delete.end();
          break;
        }
      }
      if (to_delete_it != to_delete.end())
        rel_cost -= ir::static_eval_op(func_, sorted_term->op_code(), sorted_term->operands());
    }
  }*/
  return true;
}

ir::Term *TRS::construct_term(
  const TermMatcher &matcher, const Substitution &subst, const ir::Term::PtrSet &to_delete, bool global_analysis,
  double &rel_cost, vector<size_t> &created_terms_ids)
{
  struct Call
  {
    TermMatcher::RefWrapp matcher_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  unordered_map<TermMatcher::RefWrapp, ir::Term *, TermMatcher::HashRefWrapp, TermMatcher::EqualRefWrapp> matching;

  call_stack.push(Call{matcher, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    const TermMatcher &top_matcher = top_call.matcher_;
    if (top_call.children_processed_)
    {
      if (top_matcher.is_variable())
        matching.emplace(top_matcher, subst.get(top_matcher));
      else
      {
        if (top_matcher.val())
          matching.emplace(top_matcher, func_->insert_const_term(*top_matcher.val()));
        else
        {
          vector<ir::Term *> term_operands;
          for (const auto &operand : top_matcher.operands())
          {
            if (auto matching_it = matching.find(operand); matching_it != matching.end())
              term_operands.push_back(matching_it->second);
            else
              throw logic_error("matching for term_matcher_operand was not found");
          }
          const auto &matcher_op_code = top_matcher.op_code();
          vector<int> generators_vals;
          generators_vals.reserve(matcher_op_code.generators().size());
          for (auto op_gen_matcher : matcher_op_code.generators())
            generators_vals.push_back(fold_op_gen_matcher(op_gen_matcher, subst));

          ir::OpCode term_op_code = convert_op_code(matcher_op_code, move(generators_vals));
          if (global_analysis)
          {
            //std::cout<<"===> check if cse is enabled : \n";
            if (Compiler::cse_enabled())
            {
              //std::cout<<"===> welcome in trs cse : \n";
              if (!func_->data_flow().find_op_commut(&term_op_code, &term_operands))
                rel_cost += ir::static_eval_op(func_, term_op_code, term_operands);
            }
            else
              rel_cost += ir::static_eval_op(func_, term_op_code, term_operands);
          }

          bool inserted;
          auto term = func_->insert_op_term(move(term_op_code), move(term_operands), inserted);
          //std::cout<<"===> check if cse & global_anaylsis is enabled : \n";
          if (global_analysis && Compiler::cse_enabled())
          {
            //std::cout<<"===> welcome in trs cse : \n";
            if (to_delete.find(term) != to_delete.end())
              rel_cost += ir::static_eval_op(func_, term->op_code(), term->operands());
          }
          matching.emplace(top_matcher, term);
          if (inserted)
            created_terms_ids.push_back(term->id());
        }
      }
      continue;
    }

    if (auto it = matching.find(top_matcher); it != matching.end())
      continue;

    call_stack.push(Call{top_matcher, true});
    for (const auto &operand : top_matcher.operands())
      call_stack.push(Call{operand, false});
  }
  return matching.find(matcher)->second;
}

} // namespace fheco::trs
