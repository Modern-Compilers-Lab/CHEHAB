#include "fheco/ir/expr.hpp"
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
  : op_terms_{move(other.op_terms_)}, outputs_{move(other.outputs_)}, sorted_terms_{move(other.sorted_terms_)},
    valid_top_sort_{other.valid_top_sort_}, terms_{other.terms_}
{
  other.op_terms_.clear();
  other.outputs_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();
}

Expr &Expr::operator=(Expr &&other)
{
  op_terms_ = move(other.op_terms_);
  outputs_ = move(other.outputs_);
  sorted_terms_ = move(other.sorted_terms_);
  valid_top_sort_ = other.valid_top_sort_;
  terms_ = move(other.terms_);

  other.op_terms_.clear();
  other.outputs_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();

  return *this;
}

size_t Expr::HashOpTermKey::operator()(const OpTermKey &k) const
{
  size_t h = hash<OpCode>()(*k.op_code_);
  const auto &operands = *k.operands_;
  for (auto operand : operands)
    hash_combine(h, operand);
  return h;
}

bool Expr::EqualOpTermKey::operator()(const OpTermKey &lhs, const OpTermKey &rhs) const
{
  if (*lhs.op_code_ != *rhs.op_code_)
    return false;

  const auto &lhs_operands = *lhs.operands_;
  const auto &rhs_operands = *rhs.operands_;
  if (lhs_operands.size() != rhs_operands.size())
    return false;

  for (size_t i = 0; i < lhs_operands.size(); ++i)
  {
    if (*lhs_operands[i] != *rhs_operands[i])
      return false;
  }

  return true;
}

Term *Expr::insert_op(OpCode op_code, vector<Term *> operands)
{
  Term *term = new Term(move(op_code), move(operands));
  for (auto operand : term->operands())
    operand->parents_.insert(term);
  op_terms_.emplace(OpTermKey{&term->op_code(), &term->operands()}, term);
  terms_.insert(term);
  return term;
}

Term *Expr::insert_leaf(TermType type)
{
  Term *term = new Term(move(type));
  terms_.insert(term);
  return term;
}

Term *Expr::find_term(size_t id) const
{
  Term t(id);
  if (auto it = terms_.find(&t); it != terms_.end())
    return *it;

  return nullptr;
}

Term *Expr::find_op(const OpCode &op_code, const vector<Term *> &operands) const
{
  if (auto it = op_terms_.find({&op_code, &operands}); it != op_terms_.end())
    return it->second;

  return nullptr;
}

void Expr::replace(Term *term1, Term *term2)
{
  if (*term1 == *term2)
    return;

  for (auto parent_it = term1->parents_.cbegin(); parent_it != term1->parents_.cend();)
  {
    auto parent = *parent_it;
    for (auto operand_it = parent->operands_.begin(); operand_it != parent->operands_.end(); ++operand_it)
    {
      // parent multiplicity can be used to stop once replacement is done, not to traverse the whole operands
      // vector
      auto operand = *operand_it;
      if (*operand == *term1)
        *operand_it = term2;
    }
    term2->parents_.insert(parent);
    parent_it = term1->parents_.erase(parent_it);
  }
  valid_top_sort_ = false;
}

void Expr::set_output(Term *term)
{
  if (outputs_.insert(term).second)
  {
    if (valid_top_sort_ && term->is_leaf())
    {
      for (auto sorted_term : sorted_terms_)
      {
        if (*term == *sorted_term)
          return;
      }
      sorted_terms_.push_back(term);
    }
    else
      valid_top_sort_ = false;
  }
}

void Expr::unset_output(Term *term)
{
  if (outputs_.erase(term))
  {
    if (valid_top_sort_ && term->is_leaf())
    {
      for (auto it = sorted_terms_.cbegin(); it != sorted_terms_.cend();)
      {
        auto sorted_term = *it;
        if (*term == *sorted_term)
        {
          it = sorted_terms_.erase(it);
          return;
        }
        else
          ++it;
      }
    }
    else
      valid_top_sort_ = false;
  }
}

unordered_set<size_t> Expr::prune_unreachabe_terms()
{

  struct Call
  {
    Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  unordered_set<Term *, Term::HashPtr, Term::EqualPtr> visited_terms;
  vector<Term *> all_terms_sorted;

  for (auto term : terms_)
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
        all_terms_sorted.push_back(top_term);
        continue;
      }

      if (auto it = visited_terms.find(top_term); it != visited_terms.end())
        continue;

      call_stack.push(Call{top_term, true});
      for (auto operand : top_term->operands())
        call_stack.push(Call{operand, false});
    }
  }

  unordered_set<size_t> deleted_leaves;
  for (auto it = all_terms_sorted.crbegin(); it != all_terms_sorted.crend(); ++it)
  {
    auto term = *it;
    if (can_delete(term))
    {
      if (term->is_leaf())
        deleted_leaves.insert(term->id());
      else
      {
        for (auto operand : term->operands())
          operand->parents_.erase(term);
        op_terms_.erase(OpTermKey{&term->op_code(), &term->operands()});
      }
      terms_.erase(term);
      delete term;
    }
  }
  return deleted_leaves;
}

unordered_set<size_t> Expr::delete_term_cascade(Term *term)
{
  if (!can_delete(term))
    throw invalid_argument("cannot delete term (must be source and not output)");

  struct Call
  {
    Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  unordered_set<size_t> deleted_leaves;
  call_stack.push(Call{term, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;

    if (top_call.children_processed_)
    {
      if (top_term->is_leaf())
        deleted_leaves.insert(top_term->id());
      else
        op_terms_.erase(OpTermKey{&top_term->op_code(), &top_term->operands()});
      terms_.erase(top_term);
      delete top_term;
      continue;
    }

    call_stack.push(Call{top_term, true});
    unordered_set<size_t> operands_ids;
    for (auto operand : top_term->operands())
    {
      operand->parents_.erase(top_term);
      if (can_delete(operand) && operands_ids.find(operand->id()) == operands_ids.end())
      {
        call_stack.push(Call{operand, false});
        operands_ids.insert(operand->id());
      }
    }
  }
  return deleted_leaves;
}

const vector<Term *> &Expr::get_top_sorted_terms()
{
  if (valid_top_sort_)
    return sorted_terms_;

  topological_sort();
  return sorted_terms_;
}

// iterative version of https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
void Expr::topological_sort()
{
  sorted_terms_.clear();

  struct Call
  {
    Term *term_;
    bool children_processed_;
  };
  stack<Call> call_stack;

  enum class Mark
  {
    temp,
    perm
  };
  unordered_map<Term *, Mark> terms_marks;

  for (auto term : outputs_)
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
