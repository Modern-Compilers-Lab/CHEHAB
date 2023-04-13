#pragma once

#include "op_code.hpp"
#include "term.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <set>
#include <string>
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

    Term *insert_term(std::string label, OpCode op_code, std::vector<Term *> operands);

    Term *insert_leaf(std::string label, TermType type);

    Term *find_term(const std::string &label) const;

    void delete_term(Term *t, bool cascade = false);

    void replace_term_with(Term *t1, Term *t2)
    {
      if (*t1 == *t2)
        return;

      t1->replace_with(t2);
      valid_top_sort_ = false;
    }

    std::vector<Term *> topological_sort() const;

    bool set_output(Term *t);

    bool unset_output(Term *t);

    inline const std::vector<Term *> &get_top_sorted_terms()
    {
      if (valid_top_sort_)
        return sorted_terms_;

      sorted_terms_ = topological_sort();
      valid_top_sort_ = true;
      return sorted_terms_;
    }

    inline const Term::Set &leaves() const { return leaves_; }

    inline const Term::Set &outputs() const { return outputs_; }

  private:
    Term::Set leaves_{Term::less};

    Term::Set outputs_{Term::less};

    std::vector<Term *> sorted_terms_; // nodes topologically sorted

    bool valid_top_sort_ = false;

    Term::USet terms_{0, Term::hash, Term::equal};
  };
} // namespace ir
} // namespace fhecompiler
