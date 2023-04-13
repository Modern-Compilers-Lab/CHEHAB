#include "dag.hpp"
#include <stack>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  Term *DAG::insert_term(string label, OpCode op_code, vector<Term *> operands)
  {
    if (auto term = find_term(label); term != nullptr)
      return term;

    shared_ptr<Term> term(new Term(move(label), move(op_code), move(operands)), [this](Term *t) {
      this->terms_.erase(t);
      if (auto it = outputs_.find(t); it != outputs_.end())
      {
        outputs_.erase(it);
        valid_top_sort = false;
      }
      delete t;
    });
    if (!term->is_operation())
      leaves_.emplace(term->label_, term);
    else
    {
      for (auto operand : term->operands_)
        operand->add_parent(term);
    }
    terms_.insert(term.get());
    return term.get();
  }

  Term *DAG::insert_leaf(std::string label, TermType type)
  {
    shared_ptr<Term> term = make_shared<Term>(move(label), move(type));
    leaves_.emplace(term->label_, term);
    terms_.insert(term.get());
    return term.get();
  }

  Term *DAG::find_term(const string &label) const
  {
    // temporary object used as a search key (only label matters)
    Term t(label, TermType::ciphertext);
    auto it = terms_.find(&t);
    if (it != terms_.end())
      return *it;

    return nullptr;
  }

  void DAG::delete_term(Term *t)
  {
    if (t->parents_.size() != 0)
      throw invalid_argument("cannot delete term having parents, this will invalidate the parents and the DAG");

    if (auto it = outputs_.find(t); it != outputs_.end())
    {
      outputs_.erase(it);
      valid_top_sort = false;
    }

    if (!t->is_operation())
      leaves_.erase(t->label_);
    else
    {
      for (auto operand : t->operands_)
        operand->delete_parent(t->label_);
    }
  }

  vector<Term *> DAG::topological_sort() const
  {
    vector<Term *> sorted_nodes;
    stack<pair<bool, Term *>> traversal_stack;
    unordered_set<Term *> visited_nodes;
    for (auto node : outputs_)
    {
      if (visited_nodes.find(node) == visited_nodes.end())
        traversal_stack.push(make_pair(false, node));

      while (!traversal_stack.empty())
      {
        auto top_node = traversal_stack.top();
        traversal_stack.pop();
        if (top_node.first)
        {
          sorted_nodes.push_back(top_node.second);
          continue;
        }
        if (visited_nodes.find(top_node.second) != visited_nodes.end())
          continue;

        visited_nodes.insert(top_node.second);
        traversal_stack.push(make_pair(true, top_node.second));
        if (top_node.second->is_operation())
        {
          for (auto operand : top_node.second->operands())
          {
            if (visited_nodes.find(operand) == visited_nodes.end())
              traversal_stack.push(make_pair(false, operand));
          }
        }
      }
    }
    return sorted_nodes;
  }

  bool DAG::set_output(Term *t)
  {
    if (outputs_.insert(t).second)
    {
      valid_top_sort = false;
      return true;
    }
    return false;
  }

  bool DAG::unset_output(Term *t)
  {
    if (outputs_.erase(t))
    {
      valid_top_sort = false;
      return true;
    }
    return false;
  }
} // namespace ir
} // namespace fhecompiler
