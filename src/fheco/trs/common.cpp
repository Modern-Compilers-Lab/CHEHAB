#include "fheco/trs/common.hpp"
#include "fheco/ir/common.hpp"
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

namespace fheco::trs
{
ir::OpCode convert_op_code(const TermOpCode &op_code, vector<int> generators_vals)
{
  string str_repr = op_code.str_repr();
  for (auto gen_val : generators_vals)
    str_repr += " " + to_string(gen_val);
  return ir::OpCode{op_code.type(), move(generators_vals), op_code.arity(), op_code.commutativity(), move(str_repr)};
}

int64_t evaluate_term(const ir::Term *term)
{
  if (term->type() == ir::TermType::cipher)
  {
    switch (term->op_code().type())
    {
    case ir::OpCode::Type::mul:
      if (term->operands()[0]->type() == ir::TermType::cipher && term->operands()[1]->type() == ir::TermType::cipher)
        return 100;

      else
        return 20;

    case ir::OpCode::Type::square:
      return 70;

    case ir::OpCode::Type::encrypt:
      return 50;

    case ir::OpCode::Type::rotate:
      return 25;

    case ir::OpCode::Type::add:
    case ir::OpCode::Type::sub:
    case ir::OpCode::Type::negate:
      return 1;

    case ir::OpCode::Type::nop:
      return 0;

    case ir::OpCode::Type::mod_switch:
    case ir::OpCode::Type::relin:
      throw invalid_argument("at TRS level the circuit should not contain ciphertext maintenance operations");

    default:
      throw invalid_argument("unhandled op_code resulting in a ciphertext");
    }
  }
  else
    return 0;
}

bool operator==(TermMatcherType term_matcher_type, ir::TermType term_type)
{
  switch (term_matcher_type)
  {
  case TermMatcherType::term:
    return true;

  case TermMatcherType::cipher:
    return term_type == ir::TermType::cipher;

  case TermMatcherType::plain:
    return term_type == ir::TermType::plain;

  case TermMatcherType::const_:
    throw invalid_argument("cannot verify equality of TermMatcherType::const_ with ir::TermType");

  default:
    throw invalid_argument("invalid term_matcher_type");
  }
}

void count_ctxt_leaves(ir::Term *term, TermsMetric &dp)
{
  if (term->type() != ir::TermType::cipher)
    return;

  struct Call
  {
    ir::Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  call_stack.push(Call{term, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    if (top_call.children_processed_)
    {
      if (top_term->is_leaf())
      {
        dp.emplace(top_term, 1);
        for (auto parent : top_term->parents())
          ++dp[parent];
      }
      else
      {
        for (auto parent : top_term->parents())
          dp[parent] += dp.at(top_term);
      }
      continue;
    }

    if (auto it = dp.find(top_term); it != dp.end())
    {
      for (auto parent : top_term->parents())
        dp[parent] += it->second;
      continue;
    }

    call_stack.push(Call{top_term, true});
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::TermType::cipher)
        call_stack.push(Call{operand, false});
    }
  }
}
} // namespace fheco::trs
