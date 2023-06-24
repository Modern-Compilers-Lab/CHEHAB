#include "fheco/ir/func.hpp"
#include "fheco/passes/reduce_rotation_keys.hpp"

using namespace std;

namespace fheco::passes
{
unordered_set<int> get_rotation_keys_steps(const shared_ptr<ir::Func> &func)
{
  unordered_set<int> used_keys;
  for (auto term : func->get_top_sorted_terms())
  {
    if (term->type() == ir::Term::Type::cipher && term->op_code().type() == ir::OpCode::Type::rotate)
      used_keys.insert(term->op_code().steps());
  }
  return used_keys;
}
} // namespace fheco::passes
