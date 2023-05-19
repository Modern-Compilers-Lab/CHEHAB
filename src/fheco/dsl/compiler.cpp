#include "fheco/dsl/compiler.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/cse_commut.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{
Compiler::FuncsTable Compiler::funcs_table_{};

Compiler::FuncsTable::const_iterator Compiler::active_func_it_ = Compiler::funcs_table_.cend();

bool Compiler::cse_enabled_ = true;

bool Compiler::const_folding_enabled_ = true;

void Compiler::compile(shared_ptr<ir::Func> func, bool use_mod_switch, SecurityLevel sec_level)
{
  // must remove_dead_code or trs traversal will suffer from invalidated iterators
  func->remove_dead_code();

  trs::TRS depth_opt_trs{func, trs::Ruleset::depth_opt_ruleset(func->slot_count())};
  depth_opt_trs.run(trs::TRS::RewriteHeuristic::top_down, true);

  trs::TRS ops_opt_trs{func, trs::Ruleset::ops_type_number_opt_ruleset(func->slot_count())};
  ops_opt_trs.run(trs::TRS::RewriteHeuristic::bottom_up, true);

  // passes::cse_commut(func);
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
