#include "fheco/ir/dag.hpp"
#include <stack>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{
namespace ir
{
  DAG::~DAG()
  {
    for (auto it = terms_.cbegin(); it != terms_.cend(); ++it)
      delete *it;
  }

  DAG::DAG(DAG &&other)
    : outputs_{move(other.outputs_)}, sorted_terms_{move(other.sorted_terms_)},
      valid_top_sort_{move(other.valid_top_sort_)}, terms_{move(other.terms_)}
  {
    other.outputs_.clear();
    other.sorted_terms_.clear();
    other.terms_.clear();
  }

  DAG &DAG::operator=(DAG &&other)
  {
    outputs_ = move(other.outputs_);
    sorted_terms_ = move(other.sorted_terms_);
    valid_top_sort_ = move(other.valid_top_sort_);
    terms_ = move(other.terms_);

    other.outputs_.clear();
    other.sorted_terms_.clear();
    other.terms_.clear();

    return *this;
  }

  Term *DAG::insert_operation_term(OpCode op_code, vector<Term *> operands)
  {
    Term *term = new Term(move(op_code), move(operands));
    for (auto operand : term->operands_)
      operand->parents_.emplace(Term::ParentKey{&term->op_code_, &term->operands_}, term);
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

  Term *DAG::find_term(const OpCode &op_code, const std::vector<Term *> &operands) const
  {
    if (auto it = operands[0]->parents_.find({&op_code, &operands}); it != operands[0]->parents_.end())
      return it->second;

    return nullptr;
  }

  void DAG::prune_unreachable_terms()
  {
    for (auto it = terms_.cbegin(); it != terms_.cend(); ++it)
    {
      auto t = *it;
      if (t->parents_.empty() && !is_output(t))
        delete_non_output_source_cascade(t);
    }
  }

  void DAG::replace_term_with(Term *t1, Term *t2)
  {
    if (t1->parents_.empty() || *t1 == *t2)
      return;

    t1->replace_in_parents_with(t2);
    valid_top_sort_ = false;
  }

  bool DAG::set_output(Term *t)
  {
    bool inserted = outputs_.insert(t).second;
    if (inserted)
    {
      if (!t->is_operation() && valid_top_sort_)
        sorted_terms_.push_back(t);
      else
        valid_top_sort_ = false;

      return true;
    }
    return false;
  }

  bool DAG::unset_output(Term *t)
  {
    bool erased = outputs_.erase(t);
    if (erased)
    {
      if (!t->is_operation() && valid_top_sort_)
      {
        for (auto it = sorted_terms_.cbegin(); it != sorted_terms_.cend();)
        {
          if (*t == **it)
            it = sorted_terms_.erase(it);
          else
            ++it;
        }
      }
      else
        valid_top_sort_ = false;

      return true;
    }
    return false;
  }

  void DAG::delete_non_output_source_cascade(Term *t)
  {
    for (auto operand : t->operands_)
    {
      operand->parents_.erase(Term::ParentKey{&t->op_code_, &t->operands_});
      if (operand->parents_.empty() && !is_output(operand))
        delete_non_output_source_cascade(operand);
    }
    terms_.erase(t);
    delete t;
  }

  void DAG::topological_sort()
  {
    sorted_terms_.clear();
    stack<pair<bool, Term *>> traversal_stack;
    unordered_set<Term *, HashTermPtr, EqualTermPtr> visited_nodes;
    for (auto node : outputs_)
    {
      if (visited_nodes.find(node) == visited_nodes.end())
        traversal_stack.push({false, node});

      while (!traversal_stack.empty())
      {
        auto top_node = traversal_stack.top();
        traversal_stack.pop();
        if (top_node.first)
        {
          sorted_terms_.push_back(top_node.second);
          continue;
        }
        if (visited_nodes.find(top_node.second) != visited_nodes.end())
          continue;

        visited_nodes.insert(top_node.second);
        traversal_stack.push({true, top_node.second});
        for (auto it = top_node.second->operands_.rbegin(); it != top_node.second->operands_.rend(); ++it)
        {
          if (visited_nodes.find(*it) == visited_nodes.end())
            traversal_stack.push({false, *it});
        }
      }
    }
    valid_top_sort_ = true;
  }
} // namespace ir
} // namespace fheco
