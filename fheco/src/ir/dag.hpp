#pragma once

#include "op_code.hpp"
#include "term.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class DAG
  {
  public:
    using TermSet = std::set<Term *, std::function<bool(Term *, Term *)>>;

    using TermUSet =
      std::unordered_set<Term *, std::function<std::size_t(Term *)>, std::function<bool(Term *, Term *)>>;

    DAG() = default;

    Term *insert_term(std::string label, OpCode op_code, std::vector<Term *> operands);

    Term *insert_leaf(std::string label, TermType type);

    Term *find_term(const std::string &label) const;

    void delete_term(Term *t);

    void replace_term_with(Term *t1, Term *t2)
    {
      if (*t1 == *t2)
        return;

      t1->replace_with(t2);
      valid_top_sort = false;
    }

    std::vector<Term *> topological_sort() const;

    bool set_output(Term *t);

    bool unset_output(Term *t);

    inline const std::vector<Term *> &get_top_sorted_terms()
    {
      if (valid_top_sort)
        return sorted_terms_;

      sorted_terms_ = topological_sort();
      valid_top_sort = true;
      return sorted_terms_;
    }

    inline const std::unordered_map<std::string, std::shared_ptr<Term>> &leaves() const { return leaves_; }

    inline const TermSet &outputs() const { return outputs_; }

  private:
    std::vector<Term *> sorted_terms_; // nodes topologically sorted

    bool valid_top_sort = false;

    TermUSet terms_{
      0, [](Term *e) -> std::size_t { return std::hash<std::string>{}(e->label_); },
      [](Term *lhs, Term *rhs) -> bool {
        return *lhs == *rhs;
      }};

    TermSet outputs_{[](Term *lhs, Term *rhs) -> bool {
      return *lhs < *rhs;
    }};

    // should be declared last for correct destruction
    std::unordered_map<std::string, std::shared_ptr<Term>> leaves_;
  };
} // namespace ir
} // namespace fhecompiler
