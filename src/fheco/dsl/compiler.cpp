#include "fheco/dsl/compiler.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/passes.hpp"
#include "fheco/util/common.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;

namespace fheco
{
Compiler::FuncsTable Compiler::funcs_table_{};

Compiler::FuncsTable::const_iterator Compiler::active_func_it_ = Compiler::funcs_table_.cend();

bool Compiler::cse_enabled_ = true;

bool Compiler::order_operands_enabled_ = true;

bool Compiler::const_folding_enabled_ = true;

bool Compiler::scalar_vector_shape_ = false;

void Compiler::compile(shared_ptr<ir::Func> func, bool use_mod_switch, SecurityLevel sec_level)
{
  size_t max_passes = 3;
  int64_t max_iter = 10000;

  clog << "remove_dead_code\n";
  func->remove_dead_code();

  // clog << "cse_commut\n";
  // passes::cse_commut(func);

  // clog << "halide_adapted_trs\n";
  // trs::TRS halide_adapted_trs{func, trs::Ruleset::halide_adapted_ruleset(func)};
  // halide_adapted_trs.run(trs::TRS::RewriteHeuristic::bottom_up, max_iter, false, false);

  clog << "halide_adapted_trs\n";
  trs::TRS halide_augmented_trs{func, trs::Ruleset::halide_augmented_ruleset(func)};
  halide_augmented_trs.run(trs::TRS::RewriteHeuristic::bottom_up, max_iter, false, true);

  clog << "depth_after_simplify_trs\n";
  trs::TRS depth_after_simplify_trs{func, trs::Ruleset::depth_after_simplify_ruleset(func)};
  depth_after_simplify_trs.run(trs::TRS::RewriteHeuristic::bottom_up, max_iter, false, true);

  // halide_augmented_trs.run(trs::TRS::RewriteHeuristic::bottom_up, max_iter, false, true);

  // clog << "convert_scalar_mul_to_add\n";
  // passes::convert_scalar_mul_to_add(func, 1 << 30);

  // clog << "reduce_rotation_keys\n";
  // unordered_set<int> rotation_steps_keys;
  // rotation_steps_keys =
  //   passes::reduce_rotation_keys(func, 2 * util::get_power_of_two(util::next_power_of_two(func->slot_count())));

  // clog << "insert_relin_ops\n";
  // size_t relin_keys_count = passes::lazy_relin_heuristic(func, 3);
}

void Compiler::add_func(shared_ptr<ir::Func> func)
{
  if (auto it = funcs_table_.find(func->name()); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  active_func_it_ = funcs_table_.emplace(func->name(), move(func)).first;
}

const shared_ptr<ir::Func> &Compiler::get_func(const string &name)
{
  auto it = funcs_table_.find(name);
  if (it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  return it->second;
}

void Compiler::set_active_func(const string &name)
{
  active_func_it_ = funcs_table_.find(name);
  if (active_func_it_ == funcs_table_.cend())
    throw invalid_argument("no function with this name was found");
}

void Compiler::delete_func(const string &name)
{
  if (active_func()->name() == name)
    active_func_it_ = funcs_table_.end();
  funcs_table_.erase(name);
}
} // namespace fheco
