#include "fheco/dsl/compiler.hpp"
#include <utility>

using namespace std;

namespace fheco
{
ir::Function *Compiler::active_func_ = nullptr;
unordered_map<string, ir::Function> Compiler::funcs_table_;

ir::Function &Compiler::create_func(
  std::string name, std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction, Scheme scheme)
{
  if (auto it = funcs_table_.find(name); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  if (scheme == Scheme::none)
    scheme = Scheme::bfv;

  ir::Function func(name, scheme, slot_count, modulus, signedness, delayed_reduction);
  funcs_table_.emplace(name, move(func));
  active_func_ = &funcs_table_.find(name)->second;
  return *active_func_;
}

ir::Function &Compiler::create_func(
  std::string name, std::size_t slot_count, int bit_width, bool signedness, Scheme scheme)
{
  if (auto it = funcs_table_.find(name); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  if (scheme == Scheme::none)
    scheme = Scheme::bfv;

  ir::Function func(name, scheme, slot_count, bit_width, signedness);
  funcs_table_.emplace(name, move(func));
  active_func_ = &funcs_table_.find(name)->second;
  return *active_func_;
}

void Compiler::delete_func(const string &name)
{
  if (active_func_->name() == name)
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
