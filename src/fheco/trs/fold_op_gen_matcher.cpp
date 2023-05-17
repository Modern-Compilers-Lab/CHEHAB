#include "fheco/trs/fold_op_gen_matcher.hpp"
#include <functional>
#include <stack>
#include <unordered_map>

using namespace std;

namespace fheco::trs
{
int fold_op_gen_matcher(const OpGenMatcher &matcher, const Subst &subst)
{
  struct Call
  {
    reference_wrapper<const OpGenMatcher> matcher_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  struct HashRef
  {
    size_t operator()(const reference_wrapper<const OpGenMatcher> &matcher_ref) const
    {
      return hash<OpGenMatcher>()(matcher_ref.get());
    }
  };
  struct EqualRef
  {
    bool operator()(
      const reference_wrapper<const OpGenMatcher> &lhs, const reference_wrapper<const OpGenMatcher> &rhs) const
    {
      return lhs.get() == rhs.get();
    }
  };
  unordered_map<reference_wrapper<const OpGenMatcher>, int, HashRef, EqualRef> matchers_values;

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
} // namespace fheco::trs
