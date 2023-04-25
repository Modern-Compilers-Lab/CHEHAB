#include "fheco/dsl/compiler.hpp"
#include <utility>

using namespace std;

namespace fheco
{
ir::Function *Compiler::active_func_ = nullptr;

unordered_map<string, ir::Function> Compiler::funcs_table_;

void Compiler::add_func(ir::Function func)
{
  if (auto it = funcs_table_.find(func.name()); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  active_func_ = &funcs_table_.emplace(func.name(), move(func)).first->second;
}

void Compiler::delete_func(const string &name)
{
  if (active_func().name() == name)
    active_func_ = nullptr;
  funcs_table_.erase(name);
}

void Compiler::set_active_func(const string &name)
{
  if (auto it = funcs_table_.find(name); it != funcs_table_.end())
    active_func_ = &it->second;
  else
    throw invalid_argument("no function with this name was found");
}
} // namespace fheco
