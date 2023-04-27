#pragma once

#include "fheco/ir/function.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace fheco
{
class Compiler
{
public:
  static inline void create_func(
    std::string name, std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction = false)
  {
    add_func(std::make_shared<ir::Function>(std::move(name), slot_count, modulus, signedness, delayed_reduction));
  }

  static inline void create_func(std::string name, std::size_t slot_count, int bit_width, bool signedness)
  {
    add_func(std::make_shared<ir::Function>(std::move(name), slot_count, bit_width, signedness));
  }

  static inline void create_func(
    std::string name, std::vector<std::size_t> shape, integer modulus, bool signedness, bool delayed_reduction = false)
  {
    add_func(std::make_shared<ir::Function>(std::move(name), std::move(shape), modulus, signedness, delayed_reduction));
  }

  static inline void create_func(std::string name, std::vector<std::size_t> shape, int bit_width, bool signedness)
  {
    add_func(std::make_shared<ir::Function>(std::move(name), std::move(shape), bit_width, signedness));
  }

  static inline const std::shared_ptr<ir::Function> &active_func()
  {
    if (active_func_it_ == funcs_table_.cend())
      throw std::logic_error("active_func is null");

    return active_func_it_->second;
  }

  static void set_active_func(const std::string &name);

  static const std::shared_ptr<ir::Function> &get_func(const std::string &name);

  static void delete_func(const std::string &name);

  static bool cse_enabled() { return cse_enabled_; }

  static inline void enable_cse() { cse_enabled_ = true; }

  static inline void disable_cse() { cse_enabled_ = false; }

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
  using FuncsTable = std::unordered_map<std::string, std::shared_ptr<ir::Function>>;

  static void add_func(std::shared_ptr<ir::Function> func);

  static FuncsTable funcs_table_;

  static FuncsTable::const_iterator active_func_it_;

  static bool cse_enabled_;
};
} // namespace fheco
