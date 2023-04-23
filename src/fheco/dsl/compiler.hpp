#pragma once

#include "fheco/ir/function.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace fheco
{
class Compiler
{
public:
  static ir::Function &create_func(
    std::string name, std::size_t slot_count, integer modulus, bool signedness = false, bool delayed_reduction = false,
    Scheme scheme = Scheme::bfv);

  static ir::Function &create_func(
    std::string name, std::size_t slot_count, int bit_width, bool signedness = false, Scheme scheme = Scheme::bfv);

  static void delete_func(const std::string &name);

  static inline ir::Function &get_func(const std::string &name)
  {
    auto it = funcs_table_.find(name);
    if (it == funcs_table_.end())
      throw std::invalid_argument("no function with this name was found");

    return it->second;
  }

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
  //   std::ostream &os, int trs_passes = 1, bool use_mod_switch = true, SecurityLevel sec_level =
  //   SecurityLevel::tc128)
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

private:
  static ir::Function *active_func_;

  static std::unordered_map<std::string, ir::Function> funcs_table_;
};
} // namespace fheco
