#pragma once

#include "fheco/ir/func.hpp"
#include "fheco/trs/common.hpp"
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using std::queue;
using std::string;
using namespace std;
namespace fheco
{
class Compiler_Simplification
{
public:
   static inline void enable_auto_enc_params_selection() {automatic_enc_params_enabled_ = true ;}

  static inline void disable_auto_enc_params_selection() {automatic_enc_params_enabled_ = false ;}

  static inline bool auto_enc_params_selection_enabled() {return automatic_enc_params_enabled_ ;}

  static inline const std::shared_ptr<ir::Func> &create_func(
    std::string name, std::size_t slot_count, bool delayed_reduct, integer modulus, bool signedness,
    bool need_cyclic_rotation, bool overflow_warnings = false)
  {
    return add_func(std::make_shared<ir::Func>(
      std::move(name), slot_count, delayed_reduct, modulus, signedness, need_cyclic_rotation, overflow_warnings));
  }

  static inline const std::shared_ptr<ir::Func> &create_func(
    std::string name, std::size_t slot_count, int bit_width, bool signedness, bool need_cyclic_rotation,
    bool overflow_warnings = false)
  {
    return add_func(std::make_shared<ir::Func>(
      std::move(name), slot_count, bit_width, signedness, need_cyclic_rotation, overflow_warnings));
  }

  static void compile(
    shared_ptr<ir::Func> func, ostream &header_os, string_view header_name, ostream &source_os, bool axiomatic,
    int window);

  static void gen_he_code(
    const std::shared_ptr<ir::Func> &func, std::ostream &header_os, std::string_view header_name,
    std::ostream &source_os, std::size_t rotation_keys_threshold = std::numeric_limits<std::size_t>::max(),
    bool lazy_relin = false, param_select::EncParams::SecurityLevel security_level=param_select::EncParams::SecurityLevel::tc128);

  static inline const std::shared_ptr<ir::Func> &active_func()
  {
    if (active_func_it_ == funcs_table_.cend())
      throw std::logic_error("active_func is null");

    return active_func_it_->second;
  }

  static void set_active_func(const std::string &name);

  static const std::shared_ptr<ir::Func> &get_func(const std::string &name);

  static void delete_func(const std::string &name);
  static queue<string> split(const string &s);
  static void call_equality_saturation(int slot_count, bool axiomatic);
  static ir::Term *build_expression(
    const std::shared_ptr<ir::Func> &func, std::map<string, ir::Term *> map, queue<string> &tokens);
  static ir::Term *build_expression(
    const std::shared_ptr<ir::Func> &func, std::map<string, ir::Term *> map, queue<string> &tokens, int window,
    int depth, bool axiomatic);
  static ir::OpCode operationFromString(string operation);
  static void equality_saturation_optimizer(const std::shared_ptr<ir::Func> &func, bool axiomatic);
  static void equality_saturation_optimizer(const std::shared_ptr<ir::Func> &func, bool axiomatic, int window);

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

  static const std::shared_ptr<ir::Func> &add_func(std::shared_ptr<ir::Func> func);

  static FuncsTable funcs_table_;

  static FuncsTable::const_iterator active_func_it_;

  static bool cse_enabled_;

  static bool order_operands_enabled_;

  static bool const_folding_enabled_;

  static bool scalar_vector_shape_;
  static bool automatic_enc_params_enabled_ ;

};

} // namespace fheco