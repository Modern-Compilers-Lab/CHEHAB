#pragma once

#include "fheco/ir/func.hpp"
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
    std::string name, std::size_t slot_count, integer modulus, bool signedness, bool need_full_cyclic_rotation,
    bool delayed_reduction = false, bool overflow_warnings = false)
  {
    add_func(std::make_shared<ir::Func>(
      std::move(name), slot_count, modulus, signedness, need_full_cyclic_rotation, delayed_reduction));
  }

  static inline void create_func(
    std::string name, std::size_t slot_count, int bit_width, bool signedness, bool need_full_cyclic_rotation,
    bool overflow_warnings = false)
  {
    add_func(std::make_shared<ir::Func>(
      std::move(name), slot_count, bit_width, signedness, need_full_cyclic_rotation, overflow_warnings));
  }

  static void compile(
    std::shared_ptr<ir::Func> func, bool use_mod_switch = true, SecurityLevel sec_level = SecurityLevel::tc128);

  static inline void compile(bool use_mod_switch = true, SecurityLevel sec_level = SecurityLevel::tc128)
  {
    compile(active_func(), use_mod_switch, sec_level);
  }

  static inline const std::shared_ptr<ir::Func> &active_func()
  {
    if (active_func_it_ == funcs_table_.cend())
      throw std::logic_error("active_func is null");

    return active_func_it_->second;
  }

  static void set_active_func(const std::string &name);

  static const std::shared_ptr<ir::Func> &get_func(const std::string &name);

  static void delete_func(const std::string &name);

  static inline bool cse_enabled() { return cse_enabled_; }

  static inline bool order_operands_enabled() { return order_operands_enabled_; }

  static inline bool const_folding_enabled() { return const_folding_enabled_; }

  static inline bool scalar_vector_shape_enabled() { return scalar_vector_shape_; }

  static inline void enable_cse() { cse_enabled_ = true; }

  static inline void disable_cse() { cse_enabled_ = false; }

  static inline void enable_order_operands() { order_operands_enabled_ = true; }

  static inline void disable_order_operands() { order_operands_enabled_ = false; }

  static inline void enable_const_folding() { const_folding_enabled_ = true; }

  static inline void disable_const_folding() { const_folding_enabled_ = false; }

  static inline void enable_scalar_vector_shape() { scalar_vector_shape_ = true; }

  static inline void disable_scalar_vector_shape() { scalar_vector_shape_ = false; }

private:
  using FuncsTable = std::unordered_map<std::string, std::shared_ptr<ir::Func>>;

  static void add_func(std::shared_ptr<ir::Func> func);

  static FuncsTable funcs_table_;

  static FuncsTable::const_iterator active_func_it_;

  static bool cse_enabled_;

  static bool order_operands_enabled_;

  static bool const_folding_enabled_;

  static bool scalar_vector_shape_;
};
} // namespace fheco
