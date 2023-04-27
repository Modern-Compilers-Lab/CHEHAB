#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/dag.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/util/clear_data_evaluator.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace fheco
{
namespace ir
{
  class Function
  {
  public:
    Function(
      std::string name, std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction = false);

    Function(std::string name, std::size_t slot_count, int bit_width, bool signedness)
      : Function(std::move(name), slot_count, (2 << (bit_width - 1)) - 1, signedness, true)
    {}

    Function(
      std::string name, std::vector<std::size_t> shape, integer modulus, bool signedness,
      bool delayed_reduction = false);

    Function(std::string name, std::vector<std::size_t> shape, int bit_width, bool signedness)
      : Function(std::move(name), std::move(shape), (2 << (bit_width - 1)) - 1, signedness, true)
    {}

    template <typename T>
    void init_input(T &input, std::string label);

    template <typename T>
    void init_input(T &input, std::string label, PackedVal example_val);

    template <typename T>
    void init_input(T &input, std::string label, integer example_val_slot_min, integer example_val_slot_max);

    template <typename T, typename TVal>
    void init_const(T &constant, TVal val);

    template <typename TArg, typename TDest>
    void operate_unary(OpCode op_code, const TArg &arg, TDest &dest);

    template <typename TArg1, typename TArg2, typename TDest>
    void operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest);

    inline bool is_valid_term_id(std::size_t id) const { return data_flow_.find_term(id); }

    template <typename T>
    void set_output(const T &out, std::string label);

    template <typename T>
    void set_output(const T &out, std::string label, PackedVal example_val);

    inline const std::vector<const Term *> &get_top_sorted_terms() { return data_flow_.get_top_sorted_terms(); }

    TermQualif get_term_qualif(std::size_t id) const;

    inline bool is_input_term(std::size_t id) const { return inputs_info_.find(id) != inputs_info_.end(); }

    inline bool is_const_term(std::size_t id) const { return constants_values_.find(id) != constants_values_.end(); }

    inline bool is_output_term(std::size_t id) const { return outputs_info_.find(id) != outputs_info_.end(); }

    inline const ParamTermInfo *get_input_info(std::size_t id) const
    {
      if (auto it = inputs_info_.find(id); it != inputs_info_.end())
        return &it->second;

      return nullptr;
    }

    inline const ConstVal *get_const_val(std::size_t id) const
    {
      if (auto it = constants_values_.find(id); it != constants_values_.end())
        return &it->second;

      return nullptr;
    }

    inline const ParamTermInfo *get_output_info(std::size_t id) const
    {
      if (auto it = outputs_info_.find(id); it != outputs_info_.end())
        return &it->second;

      return nullptr;
    }

    inline const std::string &name() const { return name_; }

    inline const std::vector<std::size_t> &shape() const { return shape_; }

    inline const DAG &data_flow() const { return data_flow_; }

    inline const DAG::TermPtrSet &output_terms() const { return data_flow_.output_terms(); }

    inline const IOTermsInfo &inputs_info() const { return inputs_info_; }

    inline const TermsValues &constants_values() const { return constants_values_; }

    inline const IOTermsInfo &outputs_info() const { return outputs_info_; }

    inline const util::ClearDataEvaluator &clear_data_evaluator() const { return clear_data_evaluator_; }

  private:
    static std::size_t compute_slot_count(const std::vector<std::size_t> &shape);

    std::string name_;

    std::vector<std::size_t> shape_;

    bool need_cyclic_rotations_;

    util::ClearDataEvaluator clear_data_evaluator_;

    DAG data_flow_{};

    IOTermsInfo inputs_info_{};

    TermsValues constants_values_{};

    std::unordered_map<ConstVal, std::size_t, HashConstVal> values_to_constants_;

    IOTermsInfo outputs_info_{};
  };
} // namespace ir
} // namespace fheco
