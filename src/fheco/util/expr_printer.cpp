#include "fheco/util/expr_printer.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
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

string ExprPrinter::expand_term(size_t id, Mode mode, int depth) const
{
  TermsStrExpr dp{};
  return expand_term(id, mode, depth, dp);
}

string ExprPrinter::expand_term(size_t id, Mode mode, int depth, TermsStrExpr &dp) const
{
  auto term = func_->data_flow().find_term(id);
  if (!term)
    throw invalid_argument("term with id not found");

  if (depth <= 0)
  {
    if (term->is_leaf())
      return leaf_str_expr(term);
    else
      return "$" + to_string(id);
  }

  if (auto it = dp.find(id); it != dp.end())
    return it->second;

  string result;
  if (term->is_leaf())
    result = leaf_str_expr(term);
  else
  {
    if (mode == Mode::infix || mode == Mode::infix_expl_paren)
    {
      bool expl_parenth = mode == Mode::infix_expl_paren;
      if (term->op_code().arity() == 1)
      {
        const auto arg = term->operands()[0];
        if (term->op_code().type() == ir::OpCode::Type::rotate)
        {
          if (arg->is_operation() && depth > 1)
            result = "(" + expand_term(arg->id(), mode, depth - 1, dp) + ") " + term->op_code().str_repr();
          else
            result = expand_term(arg->id(), mode, depth - 1, dp) + " " + term->op_code().str_repr();
        }
        else
        {
          if (
            arg->is_operation() && depth > 1 &&
            (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(arg->op_code().type())))
            result = term->op_code().str_repr() + "(" + expand_term(arg->id(), mode, depth - 1, dp) + ")";
          else
            result = term->op_code().str_repr() + " " + expand_term(arg->id(), mode, depth - 1, dp);
        }
      }
      else if (term->op_code().arity() == 2)
      {
        const auto lhs = term->operands()[0];
        const auto rhs = term->operands()[1];
        result = "";
        if (
          lhs->is_operation() && depth > 1 &&
          (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(lhs->op_code().type())))
          result += "(" + expand_term(lhs->id(), mode, depth - 1, dp) + ")";
        else
          result += expand_term(lhs->id(), mode, depth - 1, dp);

        result += " " + term->op_code().str_repr() + " ";

        if (
          rhs->is_operation() && depth > 1 &&
          (expl_parenth || ops_precedence_.at(term->op_code().type()) < ops_precedence_.at(rhs->op_code().type())))
          result += "(" + expand_term(rhs->id(), mode, depth - 1, dp) + ")";
        else
          result += expand_term(rhs->id(), mode, depth - 1, dp);
      }
      else
        throw invalid_argument("infix with non binary unary operation");
    }
    else if (mode == Mode::prefix)
    {
      result = term->op_code().str_repr();
      for (const auto operand : term->operands())
        result += " " + expand_term(operand->id(), mode, depth - 1, dp);
    }
    else if (mode == Mode::posfix)
    {
      result = "";
      for (const auto operand : term->operands())
        result += expand_term(operand->id(), mode, depth - 1, dp) + " ";
      result += term->op_code().str_repr();
    }
    else
      throw invalid_argument("invalid mode");
  }
  dp.emplace(id, result);
  return result;
}

string ExprPrinter::leaf_str_expr(const ir::Term *term) const
{
  if (auto input_info = func_->get_input_info(term->id()); input_info)
    return input_info->label_;
  else if (auto const_val = func_->get_const_val(term->id()); const_val)
    return "const_$" + to_string(term->id());
  else
    throw logic_error("invalid leaf term, non-input and non-const");
}

void ExprPrinter::print_outputs_str_expr(ostream &os) const
{
  os << "term_label: str_expr\n";
  for (const auto output_info : func_->outputs_info())
  {
    if (auto it = terms_str_exprs_.find(output_info.first); it != terms_str_exprs_.end())
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
