#pragma once

#include "op_code.hpp"
#include "term.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class DAG
  {
  public:
    DAG() = default;

    ~DAG();

    DAG(const DAG &other) = delete;

    DAG(DAG &&other);

    DAG &operator=(const DAG &other) = delete;

    DAG &operator=(DAG &&other);

    Term *insert_operation_term(OpCode op_code, std::vector<Term *> operands);

    Term *insert_leaf(TermType type);

    Term *find_term(std::size_t id) const;

    Term *find_term(const OpCode &op_code, const std::vector<Term *> &operands) const;

    void prune_unreachable_terms();

    void replace_term_with(Term *t1, Term *t2);

    bool set_output(Term *t);

    bool unset_output(Term *t);

    inline const std::vector<const Term *> &get_top_sorted_terms()
    {
      if (valid_top_sort_)
        return sorted_terms_;

      topological_sort();
      return sorted_terms_;
    }

  private:
    struct HashTermPtr
    {
      inline std::size_t operator()(const Term *p) const { return std::hash<Term>()(*p); }
    };

    struct EqualTermPtr
    {
      inline bool operator()(const Term *lhs, const Term *rhs) const { return *lhs == *rhs; }
    };

    struct CompareTermPtr
    {
      inline bool operator()(const Term *lhs, const Term *rhs) const { return *lhs < *rhs; }
    };

    inline bool is_output(Term *t) const { return outputs_.find(t) != outputs_.end(); }

    void delete_non_output_source_cascade(Term *t);

    void topological_sort();

    std::unordered_set<Term *, HashTermPtr, EqualTermPtr> outputs_;

    std::vector<const Term *> sorted_terms_;

    bool valid_top_sort_ = false;

    std::unordered_set<Term *, HashTermPtr, EqualTermPtr> terms_;
  };
} // namespace ir
} // namespace fhecompiler
