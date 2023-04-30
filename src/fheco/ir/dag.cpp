#include "fheco/ir/dag.hpp"
#include <stack>
#include <stdexcept>
#include <tuple>
#include <utility>

using namespace std;

namespace fheco::ir
{
DAG::~DAG()
{
  for (auto term : terms_)
    delete term;
}

DAG::DAG(DAG &&other)
  : op_terms_{move(other.op_terms_)}, outputs_{move(other.outputs_)}, sorted_terms_{move(other.sorted_terms_)},
    valid_top_sort_{other.valid_top_sort_}, terms_{other.terms_}
{
  other.op_terms_.clear();
  other.outputs_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();
}

DAG &DAG::operator=(DAG &&other)
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

size_t DAG::HashOpTermKey::operator()(const OpTermKey &k) const
{
  size_t h = hash<OpCode>()(*k.op_code_);
  const auto &operands = *k.operands_;
  for (auto operand : operands)
    hash_combine(h, operand);
  return h;
}

bool DAG::EqualOpTermKey::operator()(const OpTermKey &lhs, const OpTermKey &rhs) const
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

Term *DAG::insert_op_term(OpCode op_code, vector<Term *> operands)
{
  Term *term = new Term(move(op_code), move(operands));
  for (auto operand : term->operands_)
    operand->parents_.insert(term);
  op_terms_.emplace(OpTermKey{&term->op_code(), &term->operands()}, term);
  terms_.insert(term);
  return term;
}

Term *DAG::insert_leaf(TermType type)
{
  Term *term = new Term(move(type));
  terms_.insert(term);
  return term;
}

Term *DAG::find_term(size_t id) const
{
  Term t(id);
  if (auto it = terms_.find(&t); it != terms_.end())
    return *it;

  return nullptr;
}

Term *DAG::find_op_term(const OpCode &op_code, const vector<Term *> &operands) const
{
  if (auto it = op_terms_.find({&op_code, &operands}); it != op_terms_.end())
    return it->second;

  return nullptr;
}

void DAG::prune_unreachabe_terms()
{
  for (auto term : terms_)
  {
    if (term->is_source() && !is_output(term))
      delete_non_output_source_cascade(term);
  }
}

void DAG::delete_non_output_source_cascade(Term *term)
{
  for (auto operand : term->operands())
  {
    operand->parents_.erase(term);
    if (operand->is_source() && !is_output(operand))
      delete_non_output_source_cascade(operand);
  }
  if (term->is_operation())
    op_terms_.erase(OpTermKey{&term->op_code(), &term->operands()});
  terms_.erase(term);
  delete term;
}

void DAG::replace_term_with(Term *term1, Term *term2)
{
  if (term1->is_source() || *term1 == *term2)
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

void DAG::set_output(Term *term)
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

void DAG::unset_output(Term *term)
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

// iterative version of https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
void DAG::topological_sort()
{
  sorted_terms_.clear();
  enum class Mark
  {
    temp,
    perm
  };
  unordered_map<const Term *, Mark> terms_marks;
  struct Call
  {
    const Term *term_;
    bool childs_processed_;
  };
  stack<Call> call_stack;
  for (auto term : outputs_)
  {
    if (terms_marks.find(term) == terms_marks.end())
      call_stack.push(Call{term, false});

    while (!call_stack.empty())
    {
      auto top_call = call_stack.top();
      call_stack.pop();
      auto top_term = top_call.term_;
      if (!top_call.childs_processed_)
      {
        if (auto it = terms_marks.find(top_term); it != terms_marks.end())
        {
          if (it->second == Mark::perm)
            continue;

          if (it->second == Mark::temp)
            throw logic_error("cycle detected");
        }
        terms_marks.emplace(top_term, Mark::temp);
        call_stack.push(Call{top_term, true});
      }
      else
      {
        terms_marks[top_term] = Mark::perm;
        sorted_terms_.push_back(top_term);
        continue;
      }

      for (auto it = top_term->operands_.crbegin(); it != top_term->operands_.crend(); ++it)
        call_stack.push(Call{*it, false});
    }
  }
  valid_top_sort_ = true;
}
} // namespace fheco::ir
