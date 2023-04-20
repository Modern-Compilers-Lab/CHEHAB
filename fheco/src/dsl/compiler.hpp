#pragma once

#include "draw_ir.hpp"
// #include "evaluate_on_clear.hpp"
#include "common.hpp"
#include "function.hpp"
// #include "quantify_ir.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace fhecompiler
{
class Compiler
{
public:
  static ir::Function &create_func(
    std::string name, std::size_t vector_size, int bit_width = 16, bool signedness = true,
    ir::Scheme scheme = ir::Scheme::bfv);

  static void delete_func(const std::string &name);

  static void set_active_func(const std::string &name);

  static inline ir::Function &active_func()
  {
    if (active_func_ == nullptr)
      throw std::logic_error("active_func is null");

    return *active_func_;
  }

  // static inline void compile(
  //   const std::string &func_name, std::ostream &os, int trs_passes = 1, bool use_mod_switch = true,
  //   SecurityLevel sec_level = SecurityLevel::tc128)
  // {
  //   FuncEntry &func_entry = get_func_entry(func_name);
  //   func_entry.compile(os, trs_passes, use_mod_switch, sec_level);
  // }

  // static inline void compile(
  //   std::ostream &os, int trs_passes = 1, bool use_mod_switch = true, SecurityLevel sec_level = SecurityLevel::tc128)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   func_entry.compile(os, trs_passes, use_mod_switch, sec_level);
  // }

  // static inline void compile_noopt(
  //   const std::string &func_name, std::ostream &os, SecurityLevel sec_level = SecurityLevel::tc128)
  // {
  //   FuncEntry &func_entry = get_func_entry(func_name);
  //   func_entry.compile_noopt(os, sec_level);
  // }

  // static inline void compile_noopt(std::ostream &os, SecurityLevel sec_level = SecurityLevel::tc128)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   func_entry.compile_noopt(os, sec_level);
  // }

  static inline void draw_func_ir(const std::string &name, std::ostream &os)
  {
    FuncEntry &func_entry = get_func_entry(name);
    func_entry.draw_ir(os);
  }

  static inline void draw_func_ir(std::ostream &os)
  {
    FuncEntry &func_entry = get_func_entry(active_func().name());
    func_entry.draw_ir(os);
  }

  // static inline utils::io_variables_values evaluate_on_clear(
  //   const std::string &func_name, const utils::io_variables_values &inputs_values)
  // {
  //   FuncEntry &func_entry = get_func_entry(func_name);
  //   return func_entry.evaluate_on_clear(inputs_values);
  // }

  // static inline utils::io_variables_values evaluate_on_clear(const utils::io_variables_values &inputs_values)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   return func_entry.evaluate_on_clear(inputs_values);
  // }

  // static inline std::map<std::string, std::size_t> count_main_node_classes(const std::string &func_name)
  // {
  //   FuncEntry &func_entry = get_func_entry(func_name);
  //   return func_entry.count_main_node_classes();
  // }

  // static inline std::map<std::string, std::size_t> count_main_node_classes()
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   return func_entry.count_main_node_classes();
  // }

  // static inline void init_input(
  //   const std::string &label, const std::string &tag, long long min_value, long long max_value,
  //   ir::VectorValue &destination)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   func_entry.init_input(label, tag, min_value, max_value, destination);
  // }

  // static inline void init_input(
  //   const std::string &label, const std::string &tag, const ir::VectorValue &example_value,
  //   ir::VectorValue &destination)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   func_entry.init_input(label, tag, example_value, destination);
  // }

  // static inline void init_const(const ir::VectorValue &value, ir::VectorValue &destination)
  // {
  //   destination = utils::init_const(get_active()->get_clear_data_evaluator(), value);
  // }

  // static inline void init_const(const ir::ScalarValue &value, ir::VectorValue &destination)
  // {
  //   destination = utils::init_const_vectorize(get_active()->get_clear_data_evaluator(), value);
  // }

  // static inline void init_const(const ir::ScalarValue &value, ir::ScalarValue &destination)
  // {
  //   destination = utils::init_const(get_active()->get_clear_data_evaluator(), value);
  // }

  // static inline void operate_unary(ir::OpCode op, const ir::VectorValue &arg, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_unary(get_active()->get_clear_data_evaluator(), op, arg);
  // }

  // static inline void operate_unary(ir::OpCode op, const ir::ScalarValue &arg, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_unary_vectorize(get_active()->get_clear_data_evaluator(), op, arg);
  // }

  // static inline void operate_unary(ir::OpCode op, const ir::ScalarValue &arg, ir::ScalarValue &destination)
  // {
  //   destination = utils::operate_unary(get_active()->get_clear_data_evaluator(), op, arg);
  // }

  // static inline void operate_binary(
  //   ir::OpCode op, const ir::VectorValue &arg1, const ir::VectorValue &arg2, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_binary(get_active()->get_clear_data_evaluator(), op, arg1, arg2);
  // }

  // static void operate_binary(
  //   ir::OpCode op, const ir::VectorValue &arg1, const ir::ScalarValue &arg2, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_binary(get_active()->get_clear_data_evaluator(), op, arg1, arg2);
  // }

  // inline static void operate_binary(
  //   ir::OpCode op, const ir::ScalarValue &arg1, const ir::VectorValue &arg2, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_binary(get_active()->get_clear_data_evaluator(), op, arg1, arg2);
  // }

  // static void operate_binary(
  //   ir::OpCode op, const ir::ScalarValue &arg1, const ir::ScalarValue &arg2, ir::ScalarValue &destination)
  // {
  //   destination = utils::operate_binary(get_active()->get_clear_data_evaluator(), op, arg1, arg2);
  // }

  // static inline void operate_rotate(const ir::VectorValue &arg, int steps, ir::VectorValue &destination)
  // {
  //   destination = utils::operate_rotate(get_active()->get_clear_data_evaluator(), arg, steps);
  // }

  // static inline void set_output(const std::string &label, const std::string &tag, const ir::VectorValue &value)
  // {
  //   FuncEntry &func_entry = get_func_entry(get_active()->get_Function_tag());
  //   func_entry.set_output(label, tag, value);
  // }

  // static inline const utils::io_variables_values &get_example_input_values(const std::string &func_name)
  // {
  //   return get_func_entry(func_name).example_inputs_values;
  // }

  // static inline const utils::io_variables_values &get_example_input_values()
  // {
  //   return get_func_entry(active_func_->get_Function_tag()).example_inputs_values;
  // }

  // static inline const utils::io_variables_values &get_example_output_values(const std::string &func_name)
  // {
  //   return get_func_entry(func_name).example_outputs_values;
  // }

  // static inline const utils::io_variables_values &get_example_output_values()
  // {
  //   return get_func_entry(active_func_->get_Function_tag()).example_outputs_values;
  // }

  // static inline void print_inputs_outputs(
  //   const std::string &func_name, const utils::io_variables_values &inputs, const utils::io_variables_values
  //   &outputs, std::ostream &os)
  // {
  //   get_func_entry(func_name).print_inputs_outputs(inputs, outputs, os);
  // }

  // static inline void print_inputs_outputs(const std::string &func_name, std::ostream &os)
  // {
  //   get_func_entry(func_name).print_inputs_outputs(os);
  // }

  // static inline void print_inputs_outputs(
  //   const utils::io_variables_values &inputs, const utils::io_variables_values &outputs, std::ostream &os)
  // {
  //   get_func_entry(active_func_->get_Function_tag()).print_inputs_outputs(inputs, outputs, os);
  // }

  // static inline void print_inputs_outputs(std::ostream &os)
  // {
  //   get_func_entry(active_func_->get_Function_tag()).print_inputs_outputs(os);
  // }

private:
  struct FuncEntry
  {
    std::unique_ptr<ir::Function> func;

    // std::unordered_map<std::string, std::string> tags_labels;
    // utils::io_variables_values example_inputs_values;
    // utils::io_variables_values example_outputs_values;

    // FuncEntry(const std::shared_ptr<ir::Function> &f) : func(f) {}

    // void init_input(
    //   const std::string &label, const std::string &tag, long long min_value, long long max_value,
    //   ir::VectorValue &destination);

    // void init_input(
    //   const std::string &label, const std::string &tag, const ir::VectorValue &example_value,
    //   ir::VectorValue &destination);

    // void set_output(const std::string &label, const std::string &tag, const ir::VectorValue &value);

    // void print_inputs_outputs(
    //   const utils::io_variables_values &inputs, const utils::io_variables_values &outputs, std::ostream &os) const;

    // void print_inputs_outputs(std::ostream &os) const;

    // void compile(std::ostream &os, int trs_passes, bool use_mod_switch, SecurityLevel sec_level);

    // void compile_noopt(std::ostream &os, SecurityLevel sec_level);

    inline void draw_ir(std::ostream &os) const { util::draw_ir(*func, os); }

    // // should be const
    // inline utils::io_variables_values evaluate_on_clear(const utils::io_variables_values &inputs_values)
    // {
    //   return utils::evaluate_on_clear(func.get(), inputs_values);
    // }

    // // should be const
    // inline std::map<std::string, std::size_t> count_main_node_classes()
    // {
    //   return utils::count_main_node_classes(func.get());
    // }
  };

  static inline ir::Function &get_func(const std::string &name)
  {
    auto it = funcs_table_.find(name);
    if (it == funcs_table_.end())
      throw std::invalid_argument("no function with this name was found");

    return *it->second.func;
  }

  static inline FuncEntry &get_func_entry(const std::string &name)
  {
    auto func_entry_it = funcs_table_.find(name);
    if (func_entry_it == funcs_table_.end())
      throw std::invalid_argument("no function with label was found");

    return func_entry_it->second;
  }

  static ir::Function *active_func_;
  static std::unordered_map<std::string, FuncEntry> funcs_table_;
};
} // namespace fhecompiler
