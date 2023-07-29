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

void count_ctxt_leaves(ir::Term *term, TermsMetric &cache)
{
  if (term->type() != ir::Term::Type::cipher)
    throw invalid_argument("expected ciphertext term");

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
      if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
      {
        cache.emplace(top_term->id(), 1);
        if (top_call.parent_)
          ++interm_results[top_call.parent_->id()];
      }
      else
      {
        auto [it, inserted] = cache.emplace(top_term->id(), interm_results.at(top_term->id()));
        if (top_call.parent_)
          interm_results[top_call.parent_->id()] += it->second;
      }
      continue;
    }

    if (auto it = cache.find(top_term->id()); it != cache.end())
    {
      if (top_call.parent_)
        interm_results[top_call.parent_->id()] += it->second;
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
    cache.emplace(e.first, e.second);
}

ostream &operator<<(ostream &os, TermMatcherType term_matcher_type)
{
  switch (term_matcher_type)
  {
  case TermMatcherType::cipher:
    os << "cipher";
    break;

  case TermMatcherType::term:
    os << "term (any)";
    break;

  case TermMatcherType::plain:
    os << "plain";
    break;

  case TermMatcherType::const_:
    os << "const";
    break;

  default:
    throw invalid_argument("invalid term_matcher_type");
  }
  return os;
}

ostream &operator<<(ostream &os, RewriteHeuristic rewrite_heuristic)
{
  switch (rewrite_heuristic)
  {
  case RewriteHeuristic::bottom_up:
    os << "bottom_up";
    break;

  case RewriteHeuristic::top_down:
    os << "top_down";
    break;

  default:
    throw invalid_argument("invalid rewrite_heuristic selector");
  }
  return os;
}
} // namespace fheco::trs
