#pragma once

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

  static inline const std::shared_ptr<ir::Program> &get_active() { return active_func_; }

  static inline void compile(
    const std::string &func_name, const std::string &output_file, SecurityLevel sec_level = SecurityLevel::tc128,
    bool use_mod_switch = true)
  {
    compile(get_func(func_name), output_file, sec_level, use_mod_switch);
  }

  static inline void compile(
    const std::string &output_file, SecurityLevel sec_level = SecurityLevel::tc128, bool use_mod_switch = true)
  {
    compile(active_func_, output_file, sec_level, use_mod_switch);
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

private:
  static void compile(
    const std::shared_ptr<ir::Program> &func, const std::string &output_file, SecurityLevel sec_level,
    bool use_mod_switch);

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

    return function_it->second;
  }

  static std::shared_ptr<ir::Program> active_func_;
  static std::unordered_map<std::string, std::shared_ptr<ir::Program>> funcs_table_;
};
} // namespace fhecompiler
