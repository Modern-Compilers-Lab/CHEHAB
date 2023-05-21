#include "fheco/ir/common.hpp"
#include "fheco/trs/common.hpp"
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

bool operator==(TermMatcherType term_matcher_type, ir::Term::Type term_type)
{
  switch (term_matcher_type)
  {
  case TermMatcherType::term:
    return true;

  case TermMatcherType::cipher:
    return term_type == ir::Term::Type::cipher;

  case TermMatcherType::plain:
    return term_type == ir::Term::Type::plain;

  case TermMatcherType::const_:
    throw invalid_argument("cannot verify equality of TermMatcherType::const_ with ir::Term::Type");

  default:
    throw invalid_argument("invalid term_matcher_type");
  }
}

void count_ctxt_leaves(ir::Term *term, TermsMetric &dp)
{
  if (term->type() != ir::Term::Type::cipher)
    return;

  struct Call
  {
    ir::Term *term_;
    ir::Term *caller_parent_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  TermsMetric interm_results;
  call_stack.push(Call{term, nullptr, false});
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
        if (top_call.caller_parent_)
          ++interm_results[top_call.caller_parent_];
      }
      else
      {
        auto [it, inserted] = dp.emplace(top_term, interm_results.at(top_term));
        if (top_call.caller_parent_)
          interm_results[top_call.caller_parent_] += it->second;
      }
      continue;
    }

    if (auto it = dp.find(top_term); it != dp.end())
    {
      if (top_call.caller_parent_)
        interm_results[top_call.caller_parent_] += it->second;
      continue;
    }

    call_stack.push(Call{top_term, top_call.caller_parent_, true});
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
        call_stack.push(Call{operand, top_term, false});
    }
  }
  for (auto e : interm_results)
    dp.emplace(e.first, e.second);
}
} // namespace fheco::trs
