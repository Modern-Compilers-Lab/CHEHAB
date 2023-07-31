#include "fheco/trs/fold_op_gen_matcher.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include <cstdlib>
#include <stack>
#include <unordered_map>

using namespace std;

namespace fheco::trs
{
int fold_op_gen_matcher(const OpGenMatcher &matcher, const Substitution &subst)
{
  struct Call
  {
    OpGenMatcher::RefWrapp matcher_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  unordered_map<OpGenMatcher::RefWrapp, int, OpGenMatcher::HashRefWrapp, OpGenMatcher::EqualrRefWrapp> matchers_values;

  call_stack.push(Call{matcher, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    const OpGenMatcher &top_matcher = top_call.matcher_;
    if (top_call.children_processed_)
    {
      if (top_matcher.is_variable())
        matchers_values.emplace(top_matcher, subst.get(top_matcher));
      else
      {
        if (top_matcher.val())
          matchers_values.emplace(top_matcher, *top_matcher.val());
        else
        {
          vector<int> operands_values;
          for (const auto &operand : top_matcher.operands())
          {
            if (auto value_it = matchers_values.find(operand); value_it != matchers_values.end())
              operands_values.push_back(value_it->second);
            else
              throw logic_error("value for op_gen_matcher not found");
          }
          int val;
          operate(top_matcher.op_code(), operands_values, val);
          matchers_values.emplace(top_matcher, val);
        }
      }
      continue;
    }

    if (auto it = matchers_values.find(top_matcher); it != matchers_values.end())
      continue;

    call_stack.push(Call{top_matcher, true});
    for (const auto &operand : top_matcher.operands())
      call_stack.push(Call{operand, false});
  }
  return matchers_values.at(matcher);
}

void operate(const OpGenOpCode &op_code, const vector<int> &args, int &dest)
{
  if (op_code.arity() != args.size())
    throw invalid_argument("invalid number of args");

  switch (op_code.arity())
  {
  case 1:
    operate_unary(op_code, args[0], dest);
    break;

  case 2:
    operate_binary(op_code, args[0], args[1], dest);
    break;

  default:
    throw logic_error("unhandled op_gen_matcher folding for operations with arity > 2");
  }
}

void operate_unary(const OpGenOpCode &op_code, int arg, int &dest)
{
  switch (op_code.type())
  {
  case OpGenOpCode::Type::negate:
    dest = -arg;
    break;

  default:
    throw logic_error("unhandled op_gen_matcher folding for unary operation");
  }
}

void operate_binary(const OpGenOpCode &op_code, int arg1, int arg2, int &dest)
{
  switch (op_code.type())
  {
  case OpGenOpCode::Type::add:
    dest = arg1 + arg2;
    break;

  case OpGenOpCode::Type::sub:
    dest = arg1 - arg2;
    break;

  case OpGenOpCode::Type::mod:
    dest = arg1 % arg2;
    break;

  default:
    throw logic_error("unhandled op_gen_matcher folding for binary operation");
  }
}

OpGenMatcher fold_symbolic_op_gen_matcher(const OpGenMatcher &op_gen_matcher)
{
  auto blocks_coeffs = compute_blocks_coeffs(op_gen_matcher);
  OpGenMatcher result{};
  for (const auto &e : blocks_coeffs)
  {
    const OpGenMatcher &block = e.first;
    auto coeff = e.second;
    if (coeff > 0)
    {
      for (int i = 0; i < coeff; ++i)
      {
        if (result.id())
          result += block;
        else
          result = block;
      }
    }
    else if (coeff < 0)
    {
      for (int i = 0; i < abs(coeff); ++i)
      {
        if (result.id())
          result -= block;
        else
          result = -block;
      }
    }
  }
  return result;
}

OpGenMatcherBlocksCoeffs compute_blocks_coeffs(const OpGenMatcher &op_gen_matcher)
{
  OpGenMatcherBlocksCoeffs blocks_coeffs;
  compute_blocks_coeffs_util(op_gen_matcher, true, blocks_coeffs);
  return blocks_coeffs;
}

void compute_blocks_coeffs_util(
  const OpGenMatcher &op_gen_matcher, bool pos_neg, OpGenMatcherBlocksCoeffs &blocks_coeffs)
{
  if (op_gen_matcher.is_leaf())
  {
    if (auto it = blocks_coeffs.find(op_gen_matcher); it != blocks_coeffs.end())
    {
      if (pos_neg)
        ++it->second;
      else
        --it->second;
    }
    else
    {
      if (pos_neg)
        blocks_coeffs.emplace(op_gen_matcher, 1);
      else
        blocks_coeffs.emplace(op_gen_matcher, -1);
    }
    return;
  }
  if (op_gen_matcher.op_code().type() == OpGenOpCode::Type::add)
  {
    compute_blocks_coeffs_util(op_gen_matcher.operands()[0], pos_neg, blocks_coeffs);
    compute_blocks_coeffs_util(op_gen_matcher.operands()[1], pos_neg, blocks_coeffs);
  }
  else if (op_gen_matcher.op_code().type() == OpGenOpCode::Type::sub)
  {
    compute_blocks_coeffs_util(op_gen_matcher.operands()[0], pos_neg, blocks_coeffs);
    compute_blocks_coeffs_util(op_gen_matcher.operands()[1], !pos_neg, blocks_coeffs);
  }
  else if (op_gen_matcher.op_code().type() == OpGenOpCode::Type::negate)
    compute_blocks_coeffs_util(op_gen_matcher.operands()[0], !pos_neg, blocks_coeffs);
  else
  {
    if (pos_neg)
      blocks_coeffs.emplace(op_gen_matcher, 1);
    else
      blocks_coeffs.emplace(op_gen_matcher, -1);
  }
}

ostream &operator<<(ostream &os, const OpGenMatcherBlocksCoeffs &blocks_coeffs)
{
  for (const auto &e : blocks_coeffs)
    os << "$" << e.first.get().id() << ": " << e.second << '\n';
  return os;
}
} // namespace fheco::trs
