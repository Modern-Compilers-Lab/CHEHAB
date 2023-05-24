#include "fheco/ir/common.hpp"
#include "fheco/trs/common.hpp"
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
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

int64_t compute_xdepth(ir::Term *term)
{
  stack<pair<ir::Term *, int64_t>> dfs;
  dfs.push({term, 0});
  TermsMetric ctxt_leaves_xdepth;
  int64_t max_xdepth = 0;
  while (!dfs.empty())
  {
    auto [top_term, xdepth] = dfs.top();
    dfs.pop();
    if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
    {
      auto [it, inserted] = ctxt_leaves_xdepth.emplace(top_term, 0);
      int64_t new_xdepth = max(it->second, xdepth);
      it->second = new_xdepth;
      if (new_xdepth > max_xdepth)
        max_xdepth = new_xdepth;
      continue;
    }
    int64_t operands_xdepth = xdepth + 1;
    if (top_term->op_code().type() == ir::OpCode::Type::mul || top_term->op_code().type() == ir::OpCode::Type::square)
      ++operands_xdepth;
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
        dfs.push({operand, operands_xdepth});
    }
  }
  return max_xdepth;
}

int64_t compute_depth(ir::Term *term)
{
  stack<pair<ir::Term *, int64_t>> dfs;
  dfs.push({term, 0});
  TermsMetric ctxt_leaves_depth;
  int64_t max_depth = 0;
  while (!dfs.empty())
  {
    auto [top_term, depth] = dfs.top();
    dfs.pop();
    if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
    {
      auto [it, inserted] = ctxt_leaves_depth.emplace(top_term, 0);
      int64_t new_depth = max(it->second, depth);
      it->second = new_depth;
      if (new_depth > max_depth)
        max_depth = new_depth;
      continue;
    }
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
        dfs.push({operand, depth + 1});
    }
  }
  return max_depth;
}

int64_t count_ctxt_leaves(ir::Term *term, TermsMetric &dp)
{
  if (term->type() != ir::Term::Type::cipher)
    return 0;

  struct Call
  {
    ir::Term *term_;
    ir::Term *parent_;
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
        if (top_call.parent_)
          ++interm_results[top_call.parent_];
      }
      else
      {
        auto [it, inserted] = dp.emplace(top_term, interm_results.at(top_term));
        if (top_call.parent_)
          interm_results[top_call.parent_] += it->second;
      }
      continue;
    }

    if (auto it = dp.find(top_term); it != dp.end())
    {
      if (top_call.parent_)
        interm_results[top_call.parent_] += it->second;
      continue;
    }

    call_stack.push(Call{top_term, top_call.parent_, true});
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
        call_stack.push(Call{operand, top_term, false});
    }
  }
  for (auto e : interm_results)
    dp.emplace(e.first, e.second);

  return interm_results.at(term);
}
} // namespace fheco::trs
