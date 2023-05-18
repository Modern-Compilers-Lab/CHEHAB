#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/expr.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/util/clear_data_evaluator.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace fheco::ir
{
class Term;

class Func
{
public:
  Func(std::string name, std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction = false);

  Func(std::string name, std::size_t slot_count, int bit_width, bool signedness);

  static bool is_valid_slot_count(std::size_t slot_count);

  template <typename T>
  void init_input(T &input, std::string label);

  template <typename T>
  void init_input(T &input, std::string label, PackedVal example_val);

  template <typename T>
  void init_input(T &input, std::string label, integer example_val_slot_min, integer example_val_slot_max);

  template <typename T>
  void init_const(T &constant, PackedVal packed_val);

  template <typename TArg, typename TDest>
  void operate_unary(OpCode op_code, const TArg &arg, TDest &dest);

  template <typename TArg1, typename TArg2, typename TDest>
  void operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest);

  template <typename T>
  void set_output(const T &out, std::string label);

  template <typename T>
  void set_output(const T &out, std::string label, PackedVal example_val);

  Term *insert_const(PackedVal packed_val);

  inline Term *insert_op(OpCode op_code, std::vector<Term *> operands)
  {
    bool inserted;
    return insert_op(std::move(op_code), std::move(operands), inserted);
  }

  Term *insert_op(OpCode op_code, std::vector<Term *> operands, bool &inserted);

  Term *find_op_commut(const OpCode &op_code, const std::vector<Term *> &operands) const;

  void replace_term_with(Term *term1, Term *term2);

  void set_output(Term *term, std::string label);

  void set_output(Term *term, std::string label, PackedVal example_val);

  void remove_dead_code();

  void delete_term_cascade(Term *term);

  inline const std::vector<Term *> &get_top_sorted_terms() { return data_flow_.get_top_sorted_terms(); }

  TermQualif get_term_qualif(std::size_t id) const;

  inline bool is_valid_term_id(std::size_t id) const { return data_flow_.find_term(id); }

  inline bool is_input_term(std::size_t id) const { return inputs_info_.find(id) != inputs_info_.end(); }

  inline bool is_const_term(std::size_t id) const { return const_values_.find(id) != const_values_.end(); }

  inline bool is_output_term(std::size_t id) const { return outputs_info_.find(id) != outputs_info_.end(); }

  const ParamTermInfo *get_input_info(std::size_t id) const;

  const PackedVal *get_const_val(std::size_t id) const;

  const ParamTermInfo *get_output_info(std::size_t id) const;

  inline const std::string &name() const { return name_; }

  inline const std::size_t &slot_count() const { return slot_count_; }

  inline const util::ClearDataEvaluator &clear_data_evaluator() const { return clear_data_evaluator_; }

  inline bool need_cyclic_rotations() const { return need_cyclic_rotations_; }

  inline const Expr &data_flow() const { return data_flow_; }

  inline const IOTermsInfo &inputs_info() const { return inputs_info_; }

  inline const TermsValues &constants_values() const { return const_values_; }

  inline const IOTermsInfo &outputs_info() const { return outputs_info_; }

private:
  void clean_deleted_leaf_term(std::size_t id);

  std::string name_;

  std::size_t slot_count_;

  bool need_cyclic_rotations_;

  util::ClearDataEvaluator clear_data_evaluator_;

  Expr data_flow_{};

  IOTermsInfo inputs_info_{};

  TermsValues const_values_{};

  std::unordered_map<PackedVal, Term *, HashPackedVal> values_to_const_terms_{};

  IOTermsInfo outputs_info_{};
};
} // namespace fheco::ir
