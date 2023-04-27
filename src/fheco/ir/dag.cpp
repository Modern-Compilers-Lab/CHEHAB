#include "fheco/ir/dag.hpp"
#include <stack>
#include <stdexcept>
#include <tuple>
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
      auto term = *it;
      if (term->parents_.empty() && !is_output(term))
        delete_non_output_source_cascade(term);
    }
  }

  void DAG::replace_term_with(Term *term1, Term *term2)
  {
    if (term1->parents_.empty() || *term1 == *term2)
      return;

    term1->replace_in_parents_with(term2);
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

  void DAG::delete_non_output_source_cascade(Term *term)
  {
    for (auto operand : term->operands_)
    {
      operand->parents_.erase(Term::ParentKey{&term->op_code_, &term->operands_});
      if (operand->parents_.empty() && !is_output(operand))
        delete_non_output_source_cascade(operand);
    }
    terms_.erase(term);
    delete term;
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
    unordered_map<Term *, Mark> terms_marks;
    struct Call
    {
      Term *term_;
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

        for (auto it = top_term->operands_.rbegin(); it != top_term->operands_.rend(); ++it)
          call_stack.push(Call{*it, false});
      }
    }
    valid_top_sort_ = true;
  }
} // namespace ir
} // namespace fheco
