#pragma once

#include "clear_data_evaluator.hpp"
#include "draw_ir.hpp"
#include "evaluate_on_clear.hpp"
#include "program.hpp"
#include "quantify_ir.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace fhecompiler
{
class Compiler
{
public:
  static void create_func(
    const std::string &name, std::size_t vector_size, int bit_width = 16, bool signedness = true,
    Scheme scheme = Scheme::bfv);

  static void delete_func(const std::string &name);

  static void set_active(const std::string &func_name);

  static inline const std::shared_ptr<ir::Program> &get_active()
  {
    if (!active_func_)
      throw std::logic_error("active_func is null");

    return active_func_;
  }

  static inline void compile(
    const std::string &func_name, const std::string &output_file, int trs_passes = 1, bool use_mod_switch = true,
    SecurityLevel sec_level = SecurityLevel::tc128)
  {
    compile(get_func(func_name), output_file, trs_passes, use_mod_switch, sec_level);
  }

  static inline void compile(
    const std::string &output_file, int trs_passes = 1, bool use_mod_switch = true,
    SecurityLevel sec_level = SecurityLevel::tc128)
  {
    compile(active_func_, output_file, trs_passes, use_mod_switch, sec_level);
  }

  static inline void compile_noopt(
    const std::string &func_name, const std::string &output_file, SecurityLevel sec_level = SecurityLevel::tc128)
  {
    compile_noopt(get_func(func_name), output_file, sec_level);
  }

  static inline void compile_noopt(const std::string &output_file, SecurityLevel sec_level = SecurityLevel::tc128)
  {
    compile_noopt(active_func_, output_file, sec_level);
  }

  static inline void draw_ir(const std::string &func_name, const std::string &output_file)
  {
    draw_ir(get_func(func_name), output_file);
  }

  static inline void draw_ir(const std::string &output_file) { draw_ir(active_func_, output_file); }

  static inline utils::variables_values_map evaluate_on_clear(
    const std::string &func_name, const utils::variables_values_map &inputs_values)
  {
    return evaluate_on_clear(get_func(func_name), inputs_values);
  }

  static inline utils::variables_values_map evaluate_on_clear(const utils::variables_values_map &inputs_values)
  {
    return evaluate_on_clear(active_func_, inputs_values);
  }

  static inline std::map<std::string, std::size_t> count_main_node_classes(const std::string &func_name)
  {
    return count_main_node_classes(get_func(func_name));
  }

  static inline std::map<std::string, std::size_t> count_main_node_classes()
  {
    return count_main_node_classes(active_func_);
  }

  static inline void init_input(const std::string &label, const std::string &tag)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.init_input(label, tag);
  }

  template <typename T>
  static inline void add_const_node_value(const std::string &label, const std::vector<T> &value)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.add_const_node_value(label, clear_data_evaluator.make_element(value));
  }

  template <typename T>
  static inline void add_const_node_value(const std::string &label, T value)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.add_const_node_value(label, clear_data_evaluator.make_element(value));
  }

  static inline void operate_unary(
    ir::OpCode op, const std::string &arg, const std::string &destination, bool is_output = false)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.operate_unary(op, arg, destination, is_output);
  }

  static inline void operate_binary(
    ir::OpCode op, const std::string &arg1, const std::string &arg2, const std::string &destination,
    bool is_output = false)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.operate_binary(op, arg1, arg2, destination, is_output);
  }

  static inline void operate_rotate(
    const std::string &arg, int step, const std::string &destination, bool is_output = false)
  {
    FuncEntry &func_entry = get_func_entry(get_active()->get_program_tag());
    func_entry.operate_rotate(arg, step, destination, is_output);
  }

  static inline const utils::variables_values_map &get_input_values(const std::string &func_name)
  {
    return get_func_entry(func_name).inputs_values;
  }

  static inline const utils::variables_values_map &get_input_values()
  {
    return get_func_entry(active_func_->get_program_tag()).inputs_values;
  }

  static inline const utils::variables_values_map &get_output_values(const std::string &func_name)
  {
    return get_func_entry(func_name).outputs_values;
  }

  static inline const utils::variables_values_map &get_output_values()
  {
    return get_func_entry(active_func_->get_program_tag()).outputs_values;
  }

  static inline void serialize_inputs_outputs(
    const std::string &func_name, const utils::variables_values_map &inputs, const utils::variables_values_map &outputs,
    const std::string &file_name)
  {
    get_func_entry(func_name).serialize_inputs_outputs(inputs, outputs, file_name);
  }

  static inline void serialize_inputs_outputs(const std::string &func_name, const std::string &file_name)
  {
    get_func_entry(func_name).serialize_inputs_outputs(file_name);
  }

  static inline void serialize_inputs_outputs(
    const utils::variables_values_map &inputs, const utils::variables_values_map &outputs, const std::string &file_name)
  {
    get_func_entry(active_func_->get_program_tag()).serialize_inputs_outputs(inputs, outputs, file_name);
  }

  static inline void serialize_inputs_outputs(const std::string &file_name)
  {
    get_func_entry(active_func_->get_program_tag()).serialize_inputs_outputs(file_name);
  }

private:
  struct FuncEntry
  {
    std::shared_ptr<ir::Program> func;
    utils::variables_values_map nodes_values;
    std::unordered_map<std::string, std::string> tags_labels;
    utils::variables_values_map inputs_values;
    utils::variables_values_map outputs_values;

    const utils::variables_values_map::mapped_type &get_node_value(const std::string &label) const;

    void init_input(const std::string &label, const std::string &tag);

    void add_const_node_value(const std::string &label, const utils::variables_values_map::mapped_type &value);

    void operate_unary(ir::OpCode op, const std::string &arg, const std::string &destination, bool is_output = false);

    void operate_binary(
      ir::OpCode op, const std::string &arg1, const std::string &arg2, const std::string &destination,
      bool is_output = false);

    void operate_rotate(const std::string &arg, int step, const std::string &destination, bool is_output = false);

    void serialize_inputs_outputs(
      const utils::variables_values_map &inputs, const utils::variables_values_map &outputs,
      const std::string &file_name) const;

    void serialize_inputs_outputs(const std::string &file_name) const;
  };

  static void compile(
    const std::shared_ptr<ir::Program> &func, const std::string &output_file, int trs_passes, bool use_mod_switch,
    SecurityLevel sec_level);

  static void compile_noopt(
    const std::shared_ptr<ir::Program> &func, const std::string &output_file, SecurityLevel sec_level);

  static inline void draw_ir(const std::shared_ptr<ir::Program> &func, const std::string &output_file)
  {
    utils::draw_ir(func.get(), output_file);
  }

  static inline utils::variables_values_map evaluate_on_clear(
    const std::shared_ptr<ir::Program> &func, const utils::variables_values_map &inputs_values)
  {
    return utils::evaluate_on_clear(func.get(), inputs_values);
  }

  static inline std::map<std::string, std::size_t> count_main_node_classes(const std::shared_ptr<ir::Program> &func)
  {
    return utils::count_main_node_classes(func.get());
  }

  static inline const std::shared_ptr<ir::Program> &get_func(const std::string &func_name)
  {
    auto function_it = funcs_table_.find(func_name);
    if (function_it == funcs_table_.end())
      throw std::invalid_argument("no function with this name was found");

    return function_it->second.func;
  }

  static inline FuncEntry &get_func_entry(const std::string &label)
  {
    auto func_entry_it = funcs_table_.find(label);
    if (func_entry_it == funcs_table_.end())
      throw std::invalid_argument("no function with label was found");

    return func_entry_it->second;
  }

  static std::shared_ptr<ir::Program> active_func_;
  static std::unordered_map<std::string, FuncEntry> funcs_table_;
  static utils::ClearDataEvaluator clear_data_evaluator;
};
} // namespace fhecompiler
