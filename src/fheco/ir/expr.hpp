#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <unordered_map>
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

  Term *insert_op(OpCode op_code, std::vector<Term *> operands, bool &inserted);

  Term *insert_input(Term::Type type, InputTermInfo input_info);

  Term *insert_const(ConstInfo const_info, bool &inserted);

  Term *get_term(std::size_t id) const;

  Term *find_op_commut(const OpCode *op_code, const std::vector<Term *> *operands) const;

  Term *find_op(const OpCode *op_code, const std::vector<Term *> *operands) const;

  inline bool is_valid_term_id(std::size_t id) const { return get_term(id); }

  void replace(Term *term1, Term *term2);

  void update_term_type_cascade(Term *term);

  void set_output(const Term *term, OutputTermInfo output_term_info);

  void unset_output(const Term *term);

  void prune_unreachabe_terms();

  void delete_term_cascade(Term *term);

  void delete_terms_cascade(const Term::PtrSet &terms);

  inline bool can_delete(const Term *term) const { return term->is_source() && !is_output(term); }

  TermQualif get_qualif(const Term *term) const;

  inline bool is_input(const Term *term) const { return inputs_info_.find(term) != inputs_info_.end(); }

  inline bool is_const(const Term *term) const { return const_terms_info_.find(term) != const_terms_info_.end(); }

  inline bool is_output(const Term *term) const { return outputs_info_.find(term) != outputs_info_.end(); }

  const InputTermInfo *get_input_info(const Term *term) const;

  const PackedVal *get_const_val(const Term *term) const;

  const ConstInfo *get_const_info(const Term *term) const;

  const OutputTermInfo *get_output_info(const Term *term) const;

  const Term *get_input_term(const std::string &label) const;

  const Term *get_output_term(const std::string &label) const;

  inline const InputTermsInfo &inputs_info() const { return inputs_info_; }

  inline const ConstTermsValues &constants_info() const { return const_terms_info_; }

  inline const OutputTermsInfo &outputs_info() const { return outputs_info_; }

  const std::vector<const Term *> &get_top_sorted_terms();

  const std::vector<std::size_t> &get_top_sorted_terms_ids();

  bool valid_top_sort() const { return valid_top_sort_; }

  const Term::PtrSet &terms() const { return terms_; }

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

  InputTermsInfo inputs_info_{};

  ConstTermsValues const_terms_info_{};

  std::unordered_map<PackedVal, Term *, HashPackedVal> values_to_const_terms_{};

  OutputTermsInfo outputs_info_{};

  std::vector<const Term *> sorted_terms_{};

  std::vector<std::size_t> sorted_terms_ids_{};

  bool valid_top_sort_ = true;

  std::unordered_map<OpTermKey, Term *, HashOpTermKey, EqualOpTermKey> op_terms_{};

  Term::PtrSet terms_{};
};
} // namespace fheco::ir
