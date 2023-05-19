#include "fheco/ir/expr.hpp"
#include "fheco/dsl/compiler.hpp"
#include <algorithm>
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
  : op_terms_{move(other.op_terms_)}, outputs_info_{move(other.outputs_info_)},
    sorted_terms_{move(other.sorted_terms_)}, valid_top_sort_{other.valid_top_sort_}, terms_{other.terms_}
{
  other.op_terms_.clear();
  other.outputs_info_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();
}

Expr &Expr::operator=(Expr &&other)
{
  op_terms_ = move(other.op_terms_);
  outputs_info_ = move(other.outputs_info_);
  sorted_terms_ = move(other.sorted_terms_);
  valid_top_sort_ = other.valid_top_sort_;
  terms_ = move(other.terms_);

  other.op_terms_.clear();
  other.outputs_info_.clear();
  other.sorted_terms_.clear();
  other.terms_.clear();

  return *this;
}

size_t Expr::HashOpTermKey::operator()(const OpTermKey &k) const
{
  size_t h = hash<OpCode>()(*k.op_code_);
  const auto &operands = *k.operands_;
  for (auto operand : operands)
    hash_combine(h, *operand);
  return h;
}

bool Expr::EqualOpTermKey::operator()(const OpTermKey &lhs, const OpTermKey &rhs) const
{
  if (*lhs.op_code_ != *rhs.op_code_)
    return false;

  const auto &lhs_operands = *lhs.operands_;
  const auto &rhs_operands = *rhs.operands_;

  // terms with the same op_code_ must have the same number of operands
  for (size_t i = 0; i < lhs_operands.size(); ++i)
  {
    if (*lhs_operands[i] != *rhs_operands[i])
      return false;
  }
  return true;
}

Term *Expr::insert_op(OpCode op_code, vector<Term *> operands, bool &inserted)
{
  if (Compiler::cse_enabled())
  {
    if (auto term = find_op(op_code, operands); term)
    {
      inserted = false;
      return term;
    }
  }
  inserted = true;
  Term *term = new Term(move(op_code), move(operands));
  for (auto operand : term->operands())
    operand->parents_.insert(term);
  if (Compiler::cse_enabled())
    op_terms_.emplace(OpTermKey{&term->op_code(), &term->operands()}, term);
  terms_.insert(term);
  return term;
}

Term *Expr::insert_input(Term::Type type, ParamTermInfo input_info)
{
  Term *term = new Term(move(type));
  inputs_info_.emplace(term, move(input_info));
  terms_.insert(term);
  return term;
}

Term *Expr::insert_const(PackedVal packed_val)
{
  if (Compiler::cse_enabled())
  {
    auto it = values_to_const_terms_.find(packed_val);
    if (it != values_to_const_terms_.end())
      return it->second;
  }
  Term *term = new Term(Term::Type::plain);
  const_terms_values_.emplace(term, packed_val);
  if (Compiler::cse_enabled())
    values_to_const_terms_.emplace(move(packed_val), term);
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

Term *Expr::find_op_commut(const OpCode &op_code, const std::vector<Term *> &operands) const
{
  if (!Compiler::cse_enabled())
    throw logic_error("cse is not enabled");

  if (!op_code.commutativity())
    return find_op(op_code, operands);

  auto operands_permut = operands;
  sort(operands_permut.begin(), operands_permut.end(), Term::ComparePtr{});
  do
  {
    if (auto op_term = find_op(op_code, operands_permut); op_term)
      return op_term;
  } while (next_permutation(operands_permut.begin(), operands_permut.end(), Term::ComparePtr{}));
  return nullptr;
}

Term *Expr::find_op(const OpCode &op_code, const vector<Term *> &operands) const
{
  if (!Compiler::cse_enabled())
    throw logic_error("cse is not enabled");

  if (auto it = op_terms_.find({&op_code, &operands}); it != op_terms_.end())
    return it->second;

  return nullptr;
}

void Expr::replace(Term *term1, Term *term2)
{
  for (auto parent_it = term1->parents_.cbegin(); parent_it != term1->parents_.cend();)
  {
    auto parent = *parent_it;
    if (Compiler::cse_enabled())
      op_terms_.erase(OpTermKey{&parent->op_code(), &parent->operands()});

    for (auto operand_it = parent->operands_.begin(); operand_it != parent->operands_.end(); ++operand_it)
    {
      // parent multiplicity can be used to stop once replacement is done, not to traverse the whole operands
      // vector
      auto operand = *operand_it;
      if (*operand == *term1)
        *operand_it = term2;
    }
    if (Compiler::cse_enabled())
      op_terms_.emplace(OpTermKey{&parent->op_code(), &parent->operands()}, parent);

    term2->parents_.insert(parent);
    parent_it = term1->parents_.erase(parent_it);
  }
  valid_top_sort_ = false;
}

void Expr::set_output(Term *term, ParamTermInfo param_term_info)
{
  if (outputs_info_.emplace(term, move(param_term_info)).second)
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
  if (outputs_info_.erase(term))
  {
    if (valid_top_sort_ && term->is_leaf())
    {
      for (auto it = sorted_terms_.cbegin(); it != sorted_terms_.cend();)
      {
        auto sorted_term = *it;
        if (*term == *sorted_term)
        {
          sorted_terms_.erase(it);
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
        if (Compiler::cse_enabled())
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

  unordered_set<Term *, Term::HashPtr, Term::EqualPtr> visited_terms;
  vector<Term *> sorted_descendants;

  call_stack.push(Call{term, false});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    if (top_call.children_processed_)
    {
      visited_terms.insert(top_term);
      sorted_descendants.push_back(top_term);
      continue;
    }

    if (auto it = visited_terms.find(top_term); it != visited_terms.end())
      continue;

    call_stack.push(Call{top_term, true});
    for (auto operand : top_term->operands())
      call_stack.push(Call{operand, false});
  }

  unordered_set<size_t> deleted_leaves;
  for (auto it = sorted_descendants.crbegin(); it != sorted_descendants.crend(); ++it)
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
        if (Compiler::cse_enabled())
          op_terms_.erase(OpTermKey{&term->op_code(), &term->operands()});
      }
      terms_.erase(term);
      delete term;
    }
  }
  return deleted_leaves;
}

TermQualif Expr::get_qualif(Term *term) const
{
  if (is_input(term))
  {
    if (is_output(term))
      return TermQualif::in_out;

    return TermQualif::in;
  }

  if (is_const(term))
  {
    if (is_output(term))
      return TermQualif::const_out;

    return TermQualif::const_;
  }

  if (is_output(term))
    return TermQualif::op_out;

  return TermQualif::op;
}

const ParamTermInfo *Expr::get_input_info(Term *term) const
{
  if (auto it = inputs_info_.find(term); it != inputs_info_.end())
    return &it->second;

  return nullptr;
}

const PackedVal *Expr::get_const_val(Term *term) const
{
  if (auto it = const_terms_values_.find(term); it != const_terms_values_.end())
    return &it->second;

  return nullptr;
}

const ParamTermInfo *Expr::get_output_info(Term *term) const
{
  if (auto it = outputs_info_.find(term); it != outputs_info_.end())
    return &it->second;

  return nullptr;
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

  for (auto [term, label] : outputs_info_)
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
