#pragma once

// #include "clear_data_evaluator.hpp"
#include "common.hpp"
#include "dag.hpp"
#include "op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace fhecompiler
{
namespace ir
{
  class Function
  {
  public:
    struct ParamsInfo
    {
      std::string label;
      std::optional<VectorValue> example_value;
    };

    Function(std::string name, Scheme scheme, std::size_t vector_size, int bit_width, bool signedness);

    template <typename T>
    void init_input(T &input, std::string label);

    template <typename T>
    void init_input(T &input, std::string label, const VectorValue &example_value);

    template <typename T>
    void init_input(
      T &input, std::string label, std::int64_t example_value_slot_min, std::int64_t example_value_slot_max);

    template <typename T>
    void init_const(T &constant, const ConstantValue &value);

    template <typename TArg1, typename TArg2, typename TDestination>
    void operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDestination &destination);

    template <typename TArg, typename TDestination>
    void operate_unary(OpCode op_code, const TArg &arg, TDestination &destination);

    template <typename T>
    void set_output(const T &out, std::string label);

    inline const std::vector<const Term *> &get_top_sorted_terms() { return data_flow_.get_top_sorted_terms(); }

    TermQualif get_term_qualif(std::size_t id) const;

    inline bool is_input_term(std::size_t id) const { return inputs_info_.find(id) != inputs_info_.end(); }

    inline bool is_const_term(std::size_t id) const { return constants_values_.find(id) != constants_values_.end(); }

    inline bool is_output_term(std::size_t id) const { return outputs_info_.find(id) != outputs_info_.end(); }

    inline const ParamsInfo *get_input_info(std::size_t id) const
    {
      if (auto it = inputs_info_.find(id); it != inputs_info_.end())
        return &it->second;

      return nullptr;
    }

    inline const ConstantValue *get_const_value(std::size_t id) const
    {
      if (auto it = constants_values_.find(id); it != constants_values_.end())
        return &it->second;

      return nullptr;
    }

    inline const ParamsInfo *get_output_info(std::size_t id) const
    {
      if (auto it = outputs_info_.find(id); it != outputs_info_.end())
        return &it->second;

      return nullptr;
    }

    inline const std::string &name() const { return name_; }

    inline std::size_t vector_size() const { return vector_size_; }

  private:
    std::string name_;

    Scheme scheme_;

    std::size_t vector_size_;

    int bit_width_;

    bool signedness_;

    DAG data_flow_;

    std::unordered_map<std::size_t, ParamsInfo> inputs_info_;

    std::unordered_map<std::size_t, ConstantValue> constants_values_;

    std::unordered_map<ConstantValue, std::size_t, ConstantValueHash> values_to_constants_;

    std::unordered_map<std::size_t, ParamsInfo> outputs_info_;

    // utils::ClearDataEvaluator clear_data_evaluator_;
  };
} // namespace ir
} // namespace fhecompiler
