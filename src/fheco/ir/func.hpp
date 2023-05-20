#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/expr.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/util/clear_data_evaluator.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
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
  void init_const(T &constant, PackedVal packed_val);

  template <typename TArg, typename TDest>
  void operate_unary(OpCode op_code, const TArg &arg, TDest &dest);

  template <typename TArg1, typename TArg2, typename TDest>
  void operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest);

  template <typename T>
  void set_output(const T &out, std::string label);

  inline Term *insert_op_term(OpCode op_code, std::vector<Term *> operands)
  {
    bool inserted;
    return insert_op_term(std::move(op_code), std::move(operands), inserted);
  }

  Term *insert_op_term(OpCode op_code, std::vector<Term *> operands, bool &inserted);

  inline Term *insert_const_term(PackedVal packed_val)
  {
    bool inserted;
    return data_flow_.insert_const(std::move(packed_val), inserted);
  }

  inline Term *insert_const_term(PackedVal packed_val, bool inserted)
  {
    return data_flow_.insert_const(std::move(packed_val), inserted);
  }

  inline void replace_term_with(Term *term1, Term *term2) { data_flow_.replace(term1, term2); }

  inline void remove_dead_code() { data_flow_.prune_unreachabe_terms(); }

  inline void delete_term_cascade(Term *term) { data_flow_.delete_term_cascade(term); }

  inline const std::vector<const Term *> &get_top_sorted_terms() { return data_flow_.get_top_sorted_terms(); }

  inline const std::vector<std::size_t> &get_top_sorted_terms_ids() { return data_flow_.get_top_sorted_terms_ids(); }

  inline const std::string &name() const { return name_; }

  inline const std::size_t &slot_count() const { return slot_count_; }

  inline const util::ClearDataEvaluator &clear_data_evaluator() const { return clear_data_evaluator_; }

  inline bool need_cyclic_rotations() const { return need_cyclic_rotations_; }

  inline const Expr &data_flow() const { return data_flow_; }

private:
  std::string name_;

  std::size_t slot_count_;

  bool need_cyclic_rotations_;

  util::ClearDataEvaluator clear_data_evaluator_;

  Expr data_flow_{};
};
} // namespace fheco::ir
