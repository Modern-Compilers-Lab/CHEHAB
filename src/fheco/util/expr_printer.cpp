#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/util/expr_printer.hpp"
#include <stack>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::util
{
// for infix representation to reduce unnecessary parenthesis
const unordered_map<ir::OpCode::Type, int> ExprPrinter::ops_precedence_ = {
  {ir::OpCode::Type::encrypt, 1}, {ir::OpCode::Type::mod_switch, 1}, {ir::OpCode::Type::relin, 1},
  {ir::OpCode::Type::negate, 1},  {ir::OpCode::Type::mul, 2},        {ir::OpCode::Type::square, 2},
  {ir::OpCode::Type::add, 3},     {ir::OpCode::Type::sub, 3},        {ir::OpCode::Type::rotate, 4}};

void ExprPrinter::compute_terms_str_expr(Mode mode)
{
  terms_str_exprs_.clear();
  mode_ = mode;
  for (auto term : func_->get_top_sorted_terms())
  {
    if (term->is_leaf())
      terms_str_exprs_.emplace(term->id(), leaf_str_expr(term));
    else
    {
      if (mode_ == Mode::infix || mode_ == Mode::infix_expl_paren)
      {
        bool expl_parenth = mode_ == Mode::infix_expl_paren;
        if (term->op_code().arity() == 1)
        {
          const auto arg = term->operands()[0];
          const auto &arg_expr = terms_str_exprs_.at(arg->id());
          if (term->op_code().type() == ir::OpCode::Type::rotate)
          {
            if (arg->is_operation())
              terms_str_exprs_.emplace(term->id(), "(" + arg_expr + ") " + term->op_code().str_repr());
            else
              terms_str_exprs_.emplace(term->id(), arg_expr + " " + term->op_code().str_repr());
          }
          else
          {
            if (
              arg->is_operation() &&
              (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(arg->op_code().type())))
              terms_str_exprs_.emplace(term->id(), term->op_code().str_repr() + "(" + arg_expr + ")");
            else
              terms_str_exprs_.emplace(term->id(), term->op_code().str_repr() + " " + arg_expr);
          }
        }
        else if (term->op_code().arity() == 2)
        {
          const auto lhs = term->operands()[0];
          const auto rhs = term->operands()[1];
          const auto &lhs_expr = terms_str_exprs_.at(lhs->id());
          const auto &rhs_expr = terms_str_exprs_.at(rhs->id());
          string tmp_str_expr = "";
          if (
            lhs->is_operation() &&
            (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(lhs->op_code().type())))
            tmp_str_expr += "(" + lhs_expr + ")";
          else
            tmp_str_expr += lhs_expr;

          tmp_str_expr += " " + term->op_code().str_repr() + " ";

          if (
            rhs->is_operation() &&
            (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(rhs->op_code().type())))
            tmp_str_expr += "(" + rhs_expr + ")";
          else
            tmp_str_expr += rhs_expr;

          terms_str_exprs_.emplace(term->id(), move(tmp_str_expr));
        }
        else
          throw invalid_argument("infix with non binary unary operation");
      }
      else if (mode == Mode::prefix)
      {
        string tmp_str_expr = term->op_code().str_repr();
        for (const auto operand : term->operands())
          tmp_str_expr += " " + terms_str_exprs_.at(operand->id());
        terms_str_exprs_.emplace(term->id(), move(tmp_str_expr));
      }
      else if (mode == Mode::posfix)
      {
        string tmp_str_expr = "";
        for (const auto operand : term->operands())
          tmp_str_expr += terms_str_exprs_.at(operand->id()) + " ";
        tmp_str_expr += term->op_code().str_repr();
        terms_str_exprs_.emplace(term->id(), move(tmp_str_expr));
      }
      else
        throw invalid_argument("invalid mode");
    }
  }
}

string ExprPrinter::expand_term(const ir::Term *term, size_t depth, Mode mode) const
{
  struct Call
  {
    const ir::Term *term_;
    size_t depth_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  struct HashCall
  {
    size_t operator()(const Call &call) const
    {
      size_t h = hash<ir::Term>()(*call.term_);
      ir::hash_combine(h, call.depth_);
      return h;
    }
  };
  struct EqualCall
  {
    bool operator()(const Call &lhs, const Call &rhs) const
    {
      return *lhs.term_ == *rhs.term_ && lhs.depth_ == rhs.depth_;
    }
  };
  unordered_map<Call, string, HashCall, EqualCall> dp;

  call_stack.push(Call{term, depth, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    if (top_call.children_processed_)
    {
      string result;
      if (top_term->is_leaf())
        result = leaf_str_expr(top_term);
      else
      {
        if (mode == Mode::infix || mode == Mode::infix_expl_paren)
        {
          bool expl_parenth = mode == Mode::infix_expl_paren;
          if (top_term->op_code().arity() == 1)
          {
            const auto arg = top_term->operands()[0];
            if (top_term->op_code().type() == ir::OpCode::Type::rotate)
            {
              if (arg->is_operation() && depth > 1)
                result = "(" + dp.at(Call{arg, top_call.depth_ - 1}) + ") " + top_term->op_code().str_repr();
              else
                result = dp.at(Call{arg, top_call.depth_ - 1}) + " " + top_term->op_code().str_repr();
            }
            else
            {
              if (
                arg->is_operation() && depth > 1 &&
                (expl_parenth ||
                 ops_precedence_.at(top_term->op_code().type()) < ops_precedence_.at(arg->op_code().type())))
                result = top_term->op_code().str_repr() + "(" + dp.at(Call{arg, top_call.depth_ - 1}) + ")";
              else
                result = top_term->op_code().str_repr() + " " + dp.at(Call{arg, top_call.depth_ - 1});
            }
          }
          else if (top_term->op_code().arity() == 2)
          {
            const auto lhs = top_term->operands()[0];
            const auto rhs = top_term->operands()[1];
            result = "";
            if (
              lhs->is_operation() && depth > 1 &&
              (expl_parenth ||
               ops_precedence_.at(top_term->op_code().type()) < ops_precedence_.at(lhs->op_code().type())))
              result += "(" + dp.at(Call{lhs, top_call.depth_ - 1}) + ")";
            else
              result += dp.at(Call{lhs, top_call.depth_ - 1});

            result += " " + top_term->op_code().str_repr() + " ";

            if (
              rhs->is_operation() && depth > 1 &&
              (expl_parenth ||
               ops_precedence_.at(top_term->op_code().type()) < ops_precedence_.at(rhs->op_code().type())))
              result += "(" + dp.at(Call{rhs, top_call.depth_ - 1}) + ")";
            else
              result += dp.at(Call{rhs, top_call.depth_ - 1});
          }
          else
            throw invalid_argument("infix with non binary unary operation");
        }
        else if (mode == Mode::prefix)
        {
          result = top_term->op_code().str_repr();
          for (const auto operand : top_term->operands())
            result += " " + dp.at(Call{operand, top_call.depth_ - 1});
        }
        else if (mode == Mode::posfix)
        {
          result = "";
          for (const auto operand : top_term->operands())
            result += dp.at(Call{operand, top_call.depth_ - 1}) + " ";
          result += top_term->op_code().str_repr();
        }
        else
          throw invalid_argument("invalid mode");
      }
      dp.emplace(Call{top_term, top_call.depth_}, move(result));
      continue;
    }

    if (auto it = dp.find(Call{top_term, top_call.depth_}); it != dp.end())
      continue;

    if (top_call.depth_ == 0)
    {
      if (top_term->is_leaf())
        dp.emplace(Call{top_term, 0}, leaf_str_expr(top_term));
      else
        dp.emplace(Call{top_term, 0}, "$" + to_string(top_term->id()));

      continue;
    }

    call_stack.push(Call{top_term, top_call.depth_, true});
    for (auto operand : top_term->operands())
      call_stack.push(Call{operand, top_call.depth_ - 1, false});
  }
  return dp.at(Call{term, depth});
}

string ExprPrinter::leaf_str_expr(const ir::Term *term) const
{
  if (auto input_info = func_->data_flow().get_input_info(term); input_info)
    return input_info->label_;
  else if (auto const_val = func_->data_flow().get_const_val(term); const_val)
    return "const_$" + to_string(term->id());
  else
    throw logic_error("invalid leaf term, non-input and non-const");
}

void ExprPrinter::print_outputs_str_expr(ostream &os) const
{
  os << "term_label: str_expr\n";
  for (const auto output_info : func_->data_flow().outputs_info())
  {
    if (auto it = terms_str_exprs_.find(output_info.first->id()); it != terms_str_exprs_.end())
      os << output_info.second.label_ << ": " << it->second << '\n';
  }
}

void ExprPrinter::print_terms_str_expr(ostream &os) const
{
  os << "term_id: str_expr\n";
  os << terms_str_exprs_;
}
} // namespace fheco::util

namespace std
{
ostream &operator<<(ostream &os, const fheco::util::ExprPrinter::TermsStrExpr &terms_str_expr)
{
  for (const auto &e : terms_str_expr)
    os << '$' << e.first << ": " << e.second << '\n';
  return os;
}
} // namespace std
