#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fheco::ir

{
class DAG
{
public:
  DAG() {}

  ~DAG();

  DAG(const DAG &other) = delete;

  DAG(DAG &&other);

  DAG &operator=(const DAG &other) = delete;

  DAG &operator=(DAG &&other);

  Term *insert_op_term(OpCode op_code, std::vector<Term *> operands);

  Term *insert_leaf(TermType type);

  Term *find_term(std::size_t id) const;

  Term *find_op_term(const OpCode &op_code, const std::vector<Term *> &operands) const;

  void prune_unreachabe_terms();

  void replace_term_with(Term *term1, Term *term2);

  void set_output(Term *term);

  void unset_output(Term *term);

  inline const std::vector<const Term *> &get_top_sorted_terms()
  {
    if (valid_top_sort_)
      return sorted_terms_;

    topological_sort();
    return sorted_terms_;
  }

  const Term::PtrSet &output_terms() const { return outputs_; }

private:
  struct OpTermKey
  {
    const OpCode *op_code_;
    const std::vector<Term *> *operands_;
  };

  struct HashOpTermKey
  {
    std::size_t operator()(const OpTermKey &k) const;
  };

  struct EqualOpTermKey
  {
    bool operator()(const OpTermKey &lhs, const OpTermKey &rhs) const;
  };

  inline bool is_output(Term *term) const { return outputs_.find(term) != outputs_.end(); }

  void delete_non_output_source_cascade(Term *term);

  void topological_sort();

  std::unordered_map<OpTermKey, Term *, HashOpTermKey, EqualOpTermKey> op_terms_{};

  Term::PtrSet outputs_{};

  std::vector<const Term *> sorted_terms_{};

  bool valid_top_sort_ = true;

  Term::PtrSet terms_{};
};
} // namespace fheco::ir
