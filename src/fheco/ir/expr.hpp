#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fheco::ir
{
class Expr
{
public:
  Expr() {}

  ~Expr();

  Expr(const Expr &other) = delete;

  Expr(Expr &&other);

  Expr &operator=(const Expr &other) = delete;

  Expr &operator=(Expr &&other);

  Term *insert_op(OpCode op_code, std::vector<Term *> operands);

  Term *insert_leaf(TermType type);

  Term *find_term(std::size_t id) const;

  Term *find_op(const OpCode &op_code, const std::vector<Term *> &operands) const;

  void replace(Term *term1, Term *term2);

  void set_output(Term *term);

  void unset_output(Term *term);

  std::unordered_set<std::size_t> prune_unreachabe_terms();

  std::unordered_set<std::size_t> delete_term_cascade(Term *term);

  inline const Term::PtrSet &outputs() const { return outputs_; }

  const std::vector<Term *> &get_top_sorted_terms();

  const Term::PtrSet &terms() const { return terms_; }

  inline bool is_output(Term *term) const { return outputs_.find(term) != outputs_.end(); }

  inline bool can_delete(Term *term) const { return term->is_source() && !is_output(term); }

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

  void topological_sort();

  std::unordered_map<OpTermKey, Term *, HashOpTermKey, EqualOpTermKey> op_terms_{};

  Term::PtrSet outputs_{};

  std::vector<Term *> sorted_terms_{};

  bool valid_top_sort_ = true;

  Term::PtrSet terms_{};
};
} // namespace fheco::ir
