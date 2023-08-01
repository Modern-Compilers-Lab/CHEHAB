#include "fheco/dsl/compiler.hpp"
#include "fheco/ir/expr.hpp"
#include "fheco/util/common.hpp"
#ifdef FHECO_LOGGING
#include "fheco/util/expr_printer.hpp"
#include <iostream>
#endif
#include <algorithm>
#include <stack>
#include <stdexcept>
#include <tuple>
#include <utility>

using namespace std;

namespace fheco::ir
{
Expr::~Expr()
{
  for (auto term : terms_)
    delete term;
}

Expr::Expr(Expr &&other)
  : op_terms_{move(other.op_terms_)}, outputs_info_{move(other.outputs_info_)},
    sorted_terms_{move(other.sorted_terms_)}, valid_top_sort_{other.valid_top_sort_}, terms_{other.terms_}
{
  other.op_terms_.clear();
  other.outputs_info_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();
}

Expr &Expr::operator=(Expr &&other)
{
  op_terms_ = move(other.op_terms_);
  outputs_info_ = move(other.outputs_info_);
  sorted_terms_ = move(other.sorted_terms_);
  valid_top_sort_ = other.valid_top_sort_;
  terms_ = move(other.terms_);

  other.op_terms_.clear();
  other.outputs_info_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();

  return *this;
}

size_t Expr::HashOpTermKey::operator()(const OpTermKey &k) const
{
  size_t h = hash<OpCode>()(*k.op_code_);
  const auto &operands = *k.operands_;
  for (auto operand : operands)
    util::hash_combine(h, *operand);
  return h;
}

bool Expr::EqualOpTermKey::operator()(const OpTermKey &lhs, const OpTermKey &rhs) const
{
  if (*lhs.op_code_ != *rhs.op_code_)
    return false;

  const auto &lhs_operands = *lhs.operands_;
  const auto &rhs_operands = *rhs.operands_;

  // terms with the same op_code_ must have the same number of operands
  for (size_t i = 0; i < lhs_operands.size(); ++i)
  {
    if (*lhs_operands[i] != *rhs_operands[i])
      return false;
  }
  return true;
}

Term *Expr::insert_op(OpCode op_code, vector<Term *> operands, bool &inserted)
{
  if (Compiler::cse_enabled())
  {
    if (op_code.commutativity() && Compiler::order_operands_enabled())
      sort(operands.begin(), operands.end(), Term::ComparePtr());

    if (auto term = find_op(&op_code, &operands); term)
    {
#ifdef FHECO_LOGGING
      util::ExprPrinter expr_printer{Compiler::active_func()};
      clog << "term \"" << expr_printer.expand_term_str_expr(term) << "\" already exists (CSE)\n";
#endif
      inserted = false;
      return term;
    }
  }
  inserted = true;
  Term *term = new Term(move(op_code), move(operands));
  for (auto operand : term->operands())
    operand->parents_.insert(term);
  if (Compiler::cse_enabled())
    op_terms_.emplace(OpTermKey{&term->op_code(), &term->operands()}, term);
  terms_.insert(term);
  return term;
}

Term *Expr::insert_input(Term::Type type, InputTermInfo input_term_info)
{
  Term *term = new Term(move(type));
  inputs_info_.emplace(term, move(input_term_info));
  terms_.insert(term);
  return term;
}

Term *Expr::insert_const(ConstInfo const_info, bool &inserted)
{
  if (Compiler::cse_enabled())
  {
    auto it = values_to_const_terms_.find(const_info.val_);
    if (it != values_to_const_terms_.end())
    {
#ifdef FHECO_LOGGING
      util::ExprPrinter expr_printer{Compiler::active_func()};
      clog << "constant \"" << expr_printer.make_leaf_str_expr(it->second) << "\" already exists (CSE)\n";
#endif
      inserted = false;
      return it->second;
    }
  }
  inserted = true;
  Term *term = new Term(Term::Type::plain);

  if (Compiler::cse_enabled())
    values_to_const_terms_.emplace(const_info.val_, term);
  const_terms_info_.emplace(term, move(const_info));
  terms_.insert(term);
  return term;
}

Term *Expr::get_term(size_t id) const
{
  Term t(id);
  if (auto it = terms_.find(&t); it != terms_.end())
    return *it;

  return nullptr;
}

Term *Expr::find_op_commut(const OpCode *op_code, const vector<Term *> *operands) const
{
  if (!Compiler::cse_enabled())
    throw logic_error("cse is not enabled");

  if (!op_code->commutativity() || Compiler::order_operands_enabled())
    return find_op(op_code, operands);

  auto operands_permut = *operands;
  sort(operands_permut.begin(), operands_permut.end(), Term::ComparePtr{});
  do
  {
    if (auto op_term = find_op(op_code, &operands_permut); op_term)
      return op_term;
  } while (next_permutation(operands_permut.begin(), operands_permut.end(), Term::ComparePtr{}));
  return nullptr;
}

Term *Expr::find_op(const OpCode *op_code, const vector<Term *> *operands) const
{
  if (!Compiler::cse_enabled())
    throw logic_error("cse is not enabled");

  if (auto it = op_terms_.find(OpTermKey{op_code, operands}); it != op_terms_.end())
    return it->second;

  return nullptr;
}

void Expr::replace(Term *term1, Term *term2)
{
  if (*term1 == *term2)
    return;

  struct Call
  {
    Term *term1_;
    Term *term2_;
  };
  stack<Call> call_stack;

  call_stack.push(Call{term1, term2});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term1 = top_call.term1_;
    auto top_term2 = top_call.term2_;

    if (auto term1_output_it = outputs_info_.find(top_term1); term1_output_it != outputs_info_.end())
    {
      if (auto term2_output_it = outputs_info_.find(top_term2); term2_output_it != outputs_info_.end())
      {
        term2_output_it->second.labels_.merge(term1_output_it->second.labels_);
        outputs_info_.erase(term1_output_it);
      }
      else
      {
        outputs_info_.emplace(top_term2, move(term1_output_it->second));
        outputs_info_.erase(term1_output_it);
      }
    }

    for (auto parent_it = top_term1->parents_.cbegin(); parent_it != top_term1->parents_.cend();)
    {
      auto parent = *parent_it;
      if (*parent == *term2)
      {
        ++parent_it;
        continue;
      }

      if (Compiler::cse_enabled())
        op_terms_.erase(OpTermKey{&parent->op_code(), &parent->operands()});

      for (auto operand_it = parent->operands_.begin(); operand_it != parent->operands_.end(); ++operand_it)
      {
        // parent multiplicity can be used to stop once replacement is done, not to traverse the whole operands
        // vector
        auto operand = *operand_it;
        if (*operand == *top_term1)
          *operand_it = top_term2;
      }
      update_term_type_cascade(parent);
      if (Compiler::cse_enabled())
      {
        auto [it, inserted] = op_terms_.emplace(OpTermKey{&parent->op_code(), &parent->operands()}, parent);
        if (!inserted)
        {
          if (*it->second == *top_term1)
          {
            op_terms_.erase(it);
            it = op_terms_.emplace(OpTermKey{&parent->op_code(), &parent->operands()}, parent).first;
          }
          else
            call_stack.push(Call{parent, it->second});
        }
      }
      top_term2->parents_.insert(parent);
      parent_it = top_term1->parents_.erase(parent_it);
    }
    if (can_delete(top_term1))
      delete_term_cascade(top_term1);
  }
  valid_top_sort_ = false;
}

void Expr::update_term_type_cascade(Term *term)
{
  stack<Term *> call_stack;
  call_stack.push(term);
  while (!call_stack.empty())
  {
    auto top_term = call_stack.top();
    call_stack.pop();
    auto new_type = Term::deduce_result_type(top_term->op_code(), top_term->operands());
    if (top_term->type() != new_type)
    {
      top_term->type_ = new_type;
      for (auto parent : top_term->parents())
        call_stack.push(parent);
    }
  }
}

void Expr::set_output(const Term *term, OutputTermInfo output_term_info)
{
  if (outputs_info_.insert_or_assign(term, move(output_term_info)).second)
    valid_top_sort_ = false;
}

void Expr::unset_output(const Term *term)
{
  if (outputs_info_.erase(term))
    valid_top_sort_ = false;
}

void Expr::prune_unreachabe_terms()
{
  delete_terms_cascade(terms_);
}

void Expr::delete_term_cascade(Term *term)
{
  if (!can_delete(term))
    throw invalid_argument("cannot delete term");

  delete_terms_cascade({term});
}

void Expr::delete_terms_cascade(const Term::PtrSet &terms)
{
  struct Call
  {
    Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  Term::PtrSet visited_terms;
  vector<Term *> descendants_sorted;

  for (auto term : terms)
  {
    if (visited_terms.find(term) == visited_terms.end())
      call_stack.push(Call{term, false});

    while (!call_stack.empty())
    {
      auto top_call = call_stack.top();
      call_stack.pop();
      auto top_term = top_call.term_;
      if (top_call.children_processed_)
      {
        visited_terms.insert(top_term);
        descendants_sorted.push_back(top_term);
        continue;
      }

      if (auto it = visited_terms.find(top_term); it != visited_terms.end())
        continue;

      call_stack.push(Call{top_term, true});
      for (auto operand : top_term->operands())
        call_stack.push(Call{operand, false});
    }
  }

  for (auto it = descendants_sorted.crbegin(); it != descendants_sorted.crend(); ++it)
  {
    auto term = *it;
    if (can_delete(term))
    {
      if (term->is_leaf())
      {
        if (!inputs_info_.erase(term))
        {
          if (auto it = const_terms_info_.find(term); it != const_terms_info_.end())
          {
            if (Compiler::cse_enabled())
              values_to_const_terms_.erase(it->second.val_);
            const_terms_info_.erase(it);
          }
          else
            throw logic_error("invalid leaf, non-input and non-const");
        }
      }
      else
      {
        for (auto operand : term->operands())
          operand->parents_.erase(term);
        if (Compiler::cse_enabled())
        {
          auto it = op_terms_.find(OpTermKey{&term->op_code(), &term->operands()});
          if (*it->second == *term)
            op_terms_.erase(it);
        }
      }
      terms_.erase(term);
      delete term;
      valid_top_sort_ = false;
    }
  }
}

TermQualif Expr::get_qualif(const Term *term) const
{
  if (is_input(term))
  {
    if (is_output(term))
      return TermQualif::in_out;

    return TermQualif::in;
  }

  if (is_const(term))
  {
    if (is_output(term))
      return TermQualif::const_out;

    return TermQualif::const_;
  }

  if (is_output(term))
    return TermQualif::op_out;

  return TermQualif::op;
}

const InputTermInfo *Expr::get_input_info(const Term *term) const
{
  if (auto it = inputs_info_.find(term); it != inputs_info_.end())
    return &it->second;

  return nullptr;
}

const PackedVal *Expr::get_const_val(const Term *term) const
{
  if (auto it = const_terms_info_.find(term); it != const_terms_info_.end())
    return &it->second.val_;

  return nullptr;
}

const ConstInfo *Expr::get_const_info(const Term *term) const
{
  if (auto it = const_terms_info_.find(term); it != const_terms_info_.end())
    return &it->second;

  return nullptr;
}

const OutputTermInfo *Expr::get_output_info(const Term *term) const
{
  if (auto it = outputs_info_.find(term); it != outputs_info_.end())
    return &it->second;

  return nullptr;
}

const Term *Expr::get_input_term(const string &label) const
{
  for (const auto &[term, input_info] : inputs_info_)
  {
    if (input_info.label_ == label)
      return term;
  }
  throw invalid_argument("no input with label was found");
}

const Term *Expr::get_output_term(const string &label) const
{
  for (const auto &[term, output_info] : outputs_info_)
  {
    if (output_info.labels_.find(label) != output_info.labels_.end())
      return term;
  }
  throw invalid_argument("no output_info with label was found");
}

const vector<const Term *> &Expr::get_top_sorted_terms()
{
  if (valid_top_sort_)
    return sorted_terms_;

  topological_sort();
  return sorted_terms_;
}

const vector<size_t> &Expr::get_top_sorted_terms_ids()
{
  if (valid_top_sort_)
    return sorted_terms_ids_;

  topological_sort();
  return sorted_terms_ids_;
}

// iterative version of https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
void Expr::topological_sort()
{
  sorted_terms_.clear();
  sorted_terms_ids_.clear();

  struct Call
  {
    const Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  enum class Mark
  {
    temp,
    perm
  };
  unordered_map<const Term *, Mark, Term::HashPtr, Term::EqualPtr> terms_marks;

  for (auto [term, label] : outputs_info_)
  {
    if (terms_marks.find(term) == terms_marks.end())
      call_stack.push(Call{term, false});

    while (!call_stack.empty())
    {
      auto top_call = call_stack.top();
      call_stack.pop();
      auto top_term = top_call.term_;
      if (top_call.children_processed_)
      {
        terms_marks[top_term] = Mark::perm;
        sorted_terms_.push_back(top_term);
        sorted_terms_ids_.push_back(top_term->id());
        continue;
      }

      if (auto it = terms_marks.find(top_term); it != terms_marks.end())
      {
        if (it->second == Mark::perm)
          continue;

        if (it->second == Mark::temp)
          throw logic_error("cycle detected");
      }

      terms_marks.emplace(top_term, Mark::temp);
      call_stack.push(Call{top_term, true});
      for (auto it = top_term->operands().crbegin(); it != top_term->operands().crend(); ++it)
        call_stack.push(Call{*it, false});
    }
  }
  valid_top_sort_ = true;
}
} // namespace fheco::ir
