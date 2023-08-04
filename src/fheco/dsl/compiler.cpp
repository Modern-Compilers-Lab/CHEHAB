#include "fheco/code_gen/gen_func.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/passes.hpp"
#include "fheco/util/common.hpp"
#ifdef FHECO_LOGGING
#include <iostream>
#endif
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;

namespace fheco
{
Compiler::FuncsTable Compiler::funcs_table_{};

Compiler::FuncsTable::const_iterator Compiler::active_func_it_ = Compiler::funcs_table_.cend();

bool Compiler::cse_enabled_ = false;

bool Compiler::order_operands_enabled_ = false;

bool Compiler::const_folding_enabled_ = false;

bool Compiler::scalar_vector_shape_ = true;

void Compiler::compile(
  shared_ptr<ir::Func> func, Ruleset ruleset, trs::RewriteHeuristic rewrite_heuristic, ostream &header_os,
  string_view header_name, ostream &source_os)
{
  switch (ruleset)
  {
  case Ruleset::depth:
  {
#ifdef FHECO_LOGGING
    clog << '\n' << ruleset << "_trs" << '\n';
#endif
    trs::TRS depth_trs{trs::Ruleset::depth_ruleset(func)};
    depth_trs.run(rewrite_heuristic);
    break;
  }

  case Ruleset::ops_cost:
  {
#ifdef FHECO_LOGGING
    clog << '\n' << ruleset << "_trs" << '\n';
#endif
    trs::TRS ops_cost_trs{trs::Ruleset::ops_cost_ruleset(func)};
    ops_cost_trs.run(rewrite_heuristic);
    break;
  }

  case Ruleset::joined:
  {
#ifdef FHECO_LOGGING
    clog << '\n' << ruleset << "_trs" << '\n';
#endif
    trs::TRS joined_trs{trs::Ruleset::joined_ruleset(func)};
    joined_trs.run(rewrite_heuristic);
    break;
  }

  default:
    throw invalid_argument("invalid ruleset selector");
    break;
  }

  passes::cse_commut(func);
  gen_he_code(func, header_os, header_name, source_os);
}

void Compiler::gen_he_code(
  const std::shared_ptr<ir::Func> &func, std::ostream &header_os, std::string_view header_name, std::ostream &source_os)
{
#ifdef FHECO_LOGGING
  clog << "\nrotation_key_selection\n";
#endif
  unordered_set<int> rotation_steps_keys;
  rotation_steps_keys = passes::reduce_rotation_keys(func, 29);

#ifdef FHECO_LOGGING
  clog << "\nrelin_insertion\n";
#endif
  size_t relin_keys_count = passes::lazy_relin_heuristic(func);

#ifdef FHECO_LOGGING
  clog << "\ncode_generation\n";
#endif
  code_gen::gen_func(func, rotation_steps_keys, header_os, header_name, source_os);
}

const shared_ptr<ir::Func> &Compiler::add_func(shared_ptr<ir::Func> func)
{
  if (auto it = funcs_table_.find(func->name()); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  active_func_it_ = funcs_table_.emplace(func->name(), move(func)).first;
  return active_func_it_->second;
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

ostream &operator<<(ostream &os, Compiler::Ruleset ruleset)
{
  switch (ruleset)
  {
  case Compiler::Ruleset::depth:
    os << "depth";
    break;

  case Compiler::Ruleset::ops_cost:
    os << "ops_cost";
    break;

  case Compiler::Ruleset::joined:
    os << "joined";
    break;

  default:
    throw invalid_argument("invalid ruleset selector");
    break;
  }

  return os;
}
} // namespace fheco
