#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/expr_printer.hpp" 
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::util
{
bool startsWith(const std::string &str, const std::string &prefix)
{
  // Check if the prefix is longer than the string
  if (str.length() < prefix.length())
  {
    return false;
  }

  // Compare the first 'prefix.length()' characters of the string with the prefix
  return str.compare(0, prefix.length(), prefix) == 0;
}
std::vector<std::string> split(const std::string &str, char delimiter)
{
  std::vector<std::string> tokens;
  std::istringstream stream(str);
  std::string token;

  while (std::getline(stream, token, delimiter))
  {
    tokens.push_back(token);
  }

  return tokens;
}

// for infix representation to reduce unnecessary parenthesis
const unordered_map<ir::OpCode::Type, int> ExprPrinter::ops_precedence_ = {
  {ir::OpCode::Type::encrypt, 1}, {ir::OpCode::Type::mod_switch, 1}, {ir::OpCode::Type::relin, 1},
  {ir::OpCode::Type::negate, 1},  {ir::OpCode::Type::mul, 2},        {ir::OpCode::Type::square, 2},
  {ir::OpCode::Type::add, 3},     {ir::OpCode::Type::sub, 3},        {ir::OpCode::Type::rotate, 4},
  {ir::OpCode::Type::SumVec, 4} 
};

const unordered_map<trs::OpGenOpCode::Type, int> ExprPrinter::op_gen_matcher_ops_precedence_ = {
  {trs::OpGenOpCode::Type::negate, 1},
  {trs::OpGenOpCode::Type::mod, 2},
  {trs::OpGenOpCode::Type::add, 3},
  {trs::OpGenOpCode::Type::sub, 3}
};

string ExprPrinter::make_rule_str_repr(const trs::Rule &rule, bool show_label, Mode mode)
{
  string repr{};
  if (show_label)
    repr = rule.name() + ": ";

  repr += make_term_matcher_str_expr(rule.lhs(), mode) + " -> ";
  if (rule.has_dynamic_rhs())
    repr += "dynamic_rhs";
  else
    repr += make_term_matcher_str_expr(rule.get_rhs(), mode);
  if (rule.has_cond())
    repr += " [has_condition]";
  return repr;
}

string ExprPrinter::make_term_matcher_str_expr(const trs::TermMatcher &term_matcher, Mode mode)
{
  if (term_matcher.is_leaf())
  {
    if (term_matcher.val())
    {
      auto val = *term_matcher.val();
      if (is_scalar(val))
        return to_string(val[0]);

      return "$val" + to_string(term_matcher.id());
    }
    return *term_matcher.label();
  }

  if (mode == Mode::infix || mode == Mode::infix_expl_paren)
  {
    bool expl_parenth = mode == Mode::infix_expl_paren;
    if (term_matcher.op_code().arity() == 1)
    {
      const auto &arg = term_matcher.operands()[0];
      const auto &arg_expr = make_term_matcher_str_expr(arg, mode);
      if (term_matcher.op_code().type() == ir::OpCode::Type::rotate || term_matcher.op_code().type() == ir::OpCode::Type::SumVec)
      {
        auto steps_expr = make_op_gen_matcher_str_expr(term_matcher.op_code().steps(), mode);
        if (term_matcher.op_code().steps().is_operation())
          steps_expr = "(" + steps_expr + ")";

        if (arg.is_operation())
          return "(" + arg_expr + ") " + term_matcher.op_code().str_repr() + " " + steps_expr;
        else
          return arg_expr + " " + term_matcher.op_code().str_repr() + " " + steps_expr;
      }
      else
        return term_matcher.op_code().str_repr() + "(" + arg_expr + ")";
    }
    else if (term_matcher.op_code().arity() == 2)
    {
      const auto &lhs = term_matcher.operands()[0];
      const auto &rhs = term_matcher.operands()[1];
      const auto &lhs_expr = make_term_matcher_str_expr(lhs, mode);
      const auto &rhs_expr = make_term_matcher_str_expr(rhs, mode);
      string tmp_str_expr;
      if (
        lhs.is_operation() &&
        (expl_parenth || ops_precedence_.at(term_matcher.op_code().type()) < ops_precedence_.at(lhs.op_code().type())))
        tmp_str_expr += "(" + lhs_expr + ")";
      else
        tmp_str_expr += lhs_expr;

      tmp_str_expr += " " + term_matcher.op_code().str_repr() + " ";

      if (
        rhs.is_operation() &&
        (expl_parenth || ops_precedence_.at(term_matcher.op_code().type()) < ops_precedence_.at(rhs.op_code().type())))
        tmp_str_expr += "(" + rhs_expr + ")";
      else
        tmp_str_expr += rhs_expr;

      return tmp_str_expr;
    }
    else
      throw invalid_argument("infix with non binary unary operation");
  }
  else if (mode == Mode::prefix)
  {
    string tmp_str_expr = term_matcher.op_code().str_repr();
    for (const auto &op_gen : term_matcher.op_code().generators())
      tmp_str_expr += " " + make_op_gen_matcher_str_expr(op_gen, Mode::prefix);

    for (const auto &operand : term_matcher.operands())
      tmp_str_expr += " " + make_term_matcher_str_expr(operand, mode);
    return tmp_str_expr;
  }
  else if (mode == Mode::posfix)
  {
    string tmp_str_expr;
    for (auto operand : term_matcher.operands())
      tmp_str_expr += make_term_matcher_str_expr(operand, mode) + " ";

    tmp_str_expr += term_matcher.op_code().str_repr();
    for (const auto &op_gen : term_matcher.op_code().generators())
      tmp_str_expr += " " + make_op_gen_matcher_str_expr(op_gen, Mode::posfix);
    return tmp_str_expr;
  }
  else
    throw invalid_argument("invalid mode");
}

string ExprPrinter::make_op_gen_matcher_str_expr(const trs::OpGenMatcher &op_gen_matcher, Mode mode)
{
  if (op_gen_matcher.is_leaf())
  {
    if (op_gen_matcher.val())
      return to_string(*op_gen_matcher.val());

    return *op_gen_matcher.label();
  }

  if (mode == Mode::infix || mode == Mode::infix_expl_paren)
  {
    bool expl_parenth = mode == Mode::infix_expl_paren;
    if (op_gen_matcher.op_code().arity() == 1)
    {
      const auto &arg = op_gen_matcher.operands()[0];
      const auto &arg_expr = make_op_gen_matcher_str_expr(arg, mode);
      return op_gen_matcher.op_code().str_repr() + "(" + arg_expr + ")";
    }
    else if (op_gen_matcher.op_code().arity() == 2)
    {
      const auto &lhs = op_gen_matcher.operands()[0];
      const auto &rhs = op_gen_matcher.operands()[1];
      const auto &lhs_expr = make_op_gen_matcher_str_expr(lhs, mode);
      const auto &rhs_expr = make_op_gen_matcher_str_expr(rhs, mode);
      string tmp_str_expr;
      if (
        lhs.is_operation() && (expl_parenth || op_gen_matcher_ops_precedence_.at(op_gen_matcher.op_code().type()) <
                                                 op_gen_matcher_ops_precedence_.at(lhs.op_code().type())))
        tmp_str_expr += "(" + lhs_expr + ")";
      else
        tmp_str_expr += lhs_expr;

      tmp_str_expr += " " + op_gen_matcher.op_code().str_repr() + " ";

      if (
        rhs.is_operation() && (expl_parenth || op_gen_matcher_ops_precedence_.at(op_gen_matcher.op_code().type()) <
                                                 op_gen_matcher_ops_precedence_.at(rhs.op_code().type())))
        tmp_str_expr += "(" + rhs_expr + ")";
      else
        tmp_str_expr += rhs_expr;

      return tmp_str_expr;
    }
    else
      throw invalid_argument("infix with non binary unary operation");
  }
  else if (mode == Mode::prefix)
  {
    string tmp_str_expr = op_gen_matcher.op_code().str_repr();
    for (const auto &operand : op_gen_matcher.operands())
      tmp_str_expr += " " + make_op_gen_matcher_str_expr(operand, mode);
    return tmp_str_expr;
  }
  else if (mode == Mode::posfix)
  {
    string tmp_str_expr;
    for (auto operand : op_gen_matcher.operands())
      tmp_str_expr += make_op_gen_matcher_str_expr(operand, mode) + " ";
    tmp_str_expr += op_gen_matcher.op_code().str_repr();
    return tmp_str_expr;
  }
  else
    throw invalid_argument("invalid mode");
}
/******************************************************************************************/
/*****************************************************************************************/
/******************************************************************************************/
void ExprPrinter::make_terms_str_expr(Mode mode)
{
  terms_str_exprs_.clear();
  mode_ = mode;
  for (auto term : func_->get_top_sorted_terms())
  {
    if (term->is_leaf())
    {

      terms_str_exprs_.emplace(term->id(), make_leaf_str_expr(term));
    }
    else
    {
      if (mode_ == Mode::infix || mode_ == Mode::infix_expl_paren)
      {
        bool expl_parenth = mode_ == Mode::infix_expl_paren;
        if (term->op_code().arity() == 1)
        {
          auto arg = term->operands()[0];
          const auto &arg_expr = terms_str_exprs_.at(arg->id());
          if (term->op_code().type() == ir::OpCode::Type::rotate || term->op_code().type() == ir::OpCode::Type::SumVec)
          {
            if (arg->is_operation())
              terms_str_exprs_.emplace(term->id(), "(" + arg_expr + ") " + term->op_code().str_repr());
            else
              terms_str_exprs_.emplace(term->id(), arg_expr + " " + term->op_code().str_repr());
          }
          else
            terms_str_exprs_.emplace(term->id(), term->op_code().str_repr() + "(" + arg_expr + ")");
        }
        else if (term->op_code().arity() == 2)
        {
          auto lhs = term->operands()[0];
          auto rhs = term->operands()[1];
          const auto &lhs_expr = terms_str_exprs_.at(lhs->id());
          const auto &rhs_expr = terms_str_exprs_.at(rhs->id());
          string tmp_str_expr{};
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
        string opcode = term->op_code().str_repr();
        string tmp_str_expr;
        vector<string> operands;
        for (auto operand : term->operands())
          operands.push_back(terms_str_exprs_.at(operand->id()));
        if (startsWith(opcode, "<<"))
        {
          auto op = split(opcode, ' ');
          tmp_str_expr = "( " + op[0] + " " + operands[0] + " " + op[1] + " )";
        }else if (startsWith(opcode, "SumVec")){
          auto op = split(opcode, ' ');
          tmp_str_expr = "( " + op[0] + " " + operands[0] + " " + op[1] + " )";
        }
        else
        {
          if (opcode == "negate")
          {
            opcode = "-";
          }
          tmp_str_expr = "( " + opcode;
          for (auto operand : operands)
          {
            tmp_str_expr += " " + operand;
          }
          tmp_str_expr += " )";
        }
        terms_str_exprs_.emplace(term->id(), move(tmp_str_expr));
      }
      else if (mode == Mode::posfix)
      {
        string tmp_str_expr{};
        for (auto operand : term->operands())
          tmp_str_expr += terms_str_exprs_.at(operand->id()) + " ";
        tmp_str_expr += term->op_code().str_repr();
        terms_str_exprs_.emplace(term->id(), move(tmp_str_expr));
      }
      else
        throw invalid_argument("invalid mode");
    }
  }
}
/*************************************************************************************/
/************************************************************************************/
/*************************************************************************************** */
void ExprPrinter::print_expand_outputs_str_expr(ostream &os, int depth, Mode mode) const
{
  os << "output: expression\n";
  for (auto output_info : func_->data_flow().outputs_info())
  {
    auto str_expr = expand_term_str_expr(output_info.first, depth, mode);
    for (const auto &label : output_info.second.labels_)
      os << label << ": " << str_expr << '\n';
  }
}

string ExprPrinter::expand_term_str_expr(const ir::Term *term, int depth, Mode mode) const
{
  struct Call
  {
    const ir::Term *term_;
    int depth_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  struct HashCall
  {
    size_t operator()(const Call &call) const
    {
      size_t h = hash<ir::Term>()(*call.term_);
      util::hash_combine(h, call.depth_);
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
  unordered_map<Call, string, HashCall, EqualCall> computed_calls;

  call_stack.push(Call{term, depth, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    if (top_call.children_processed_)
    {
      string result{};
      if (top_term->is_leaf())
        result = make_leaf_str_expr(top_term);
      else
      {
        if (mode == Mode::infix || mode == Mode::infix_expl_paren)
        {
          bool expl_parenth = mode == Mode::infix_expl_paren;
          if (top_term->op_code().arity() == 1)
          {
            auto arg = top_term->operands()[0];
            if (top_term->op_code().type() == ir::OpCode::Type::rotate || top_term->op_code().type() == ir::OpCode::Type::SumVec )
            {
              if (arg->is_operation())
                result =
                  "(" + computed_calls.at(Call{arg, top_call.depth_ - 1}) + ") " + top_term->op_code().str_repr();
              else
                result = computed_calls.at(Call{arg, top_call.depth_ - 1}) + " " + top_term->op_code().str_repr();
            }
            else
              result = top_term->op_code().str_repr() + "(" + computed_calls.at(Call{arg, top_call.depth_ - 1}) + ")";
          }
          else if (top_term->op_code().arity() == 2)
          {
            auto lhs = top_term->operands()[0];
            auto rhs = top_term->operands()[1];
            if (
              lhs->is_operation() && (expl_parenth || ops_precedence_.at(top_term->op_code().type()) <
                                                        ops_precedence_.at(lhs->op_code().type())))
              result += "(" + computed_calls.at(Call{lhs, top_call.depth_ - 1}) + ")";
            else
              result += computed_calls.at(Call{lhs, top_call.depth_ - 1});

            result += " " + top_term->op_code().str_repr() + " ";

            if (
              rhs->is_operation() && (expl_parenth || ops_precedence_.at(top_term->op_code().type()) <
                                                        ops_precedence_.at(rhs->op_code().type())))
              result += "(" + computed_calls.at(Call{rhs, top_call.depth_ - 1}) + ")";
            else
              result += computed_calls.at(Call{rhs, top_call.depth_ - 1});
          }
          else
            throw invalid_argument("infix with non binary unary operation");
        }
        else if (mode == Mode::prefix)
        {
          result = top_term->op_code().str_repr();
          for (auto operand : top_term->operands())
            result += " " + computed_calls.at(Call{operand, top_call.depth_ - 1});
        }
        else if (mode == Mode::posfix)
        {
          for (auto operand : top_term->operands())
            result += computed_calls.at(Call{operand, top_call.depth_ - 1}) + " ";
          result += top_term->op_code().str_repr();
        }
        else
          throw invalid_argument("invalid mode");
      }
      computed_calls.emplace(Call{top_term, top_call.depth_}, move(result));
      continue;
    }

    if (auto it = computed_calls.find(Call{top_term, top_call.depth_}); it != computed_calls.end())
      continue;

    if (top_call.depth_ <= 0)
    {
      if (top_term->is_leaf())
        computed_calls.emplace(Call{top_term, 0}, make_leaf_str_expr(top_term));
      else
        computed_calls.emplace(Call{top_term, 0}, "$" + to_string(top_term->id()));

      continue;
    }

    call_stack.push(Call{top_term, top_call.depth_, true});
    for (auto operand : top_term->operands())
      call_stack.push(Call{operand, top_call.depth_ - 1, false});
  }
  return computed_calls.at(Call{term, depth});
}

string ExprPrinter::make_leaf_str_expr(const ir::Term *term) const
{
  if (auto input_info = func_->data_flow().get_input_info(term); input_info)
    return input_info->label_;
  else if (auto const_info = func_->data_flow().get_const_info(term); const_info)
  {
    if (const_info->is_scalar_)
      return to_string(const_info->val_[0]);

    return "const_$" + to_string(term->id());
  }
  else
    throw logic_error("invalid leaf term, non-input and non-const");
}

void ExprPrinter::print_outputs_str_expr(ostream &os) const
{
  os << "output: expression\n";
  for (auto output_info : func_->data_flow().outputs_info())
  {
    if (auto it = terms_str_exprs_.find(output_info.first->id()); it != terms_str_exprs_.end())
    {
      for (const auto &label : output_info.second.labels_)
        os << label << ": " << it->second << '\n';
    }
  }
}

void ExprPrinter::print_terms_str_expr(ostream &os) const
{
  os << "term_id: expression\n";
  os << terms_str_exprs_;
}

ostream &operator<<(ostream &os, const ExprPrinter::TermsStrExpr &terms_str_expr)
{
  for (const auto &e : terms_str_expr)
    os << '$' << e.first << ": " << e.second << '\n';
  return os;
}
} // namespace fheco::util