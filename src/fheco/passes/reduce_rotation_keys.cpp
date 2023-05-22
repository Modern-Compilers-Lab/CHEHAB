#include "fheco/ir/func.hpp"
#include "fheco/passes/reduce_rotation_keys.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace fheco::passes
{
unordered_set<int> reduce_rotation_keys(const shared_ptr<ir::Func> &func, size_t keys_threshold)
{
  unordered_set<int> used_keys;
  for (auto term : func->get_top_sorted_terms())
  {
    if (term->type() == ir::Term::Type::cipher && term->op_code().type() == ir::OpCode::Type::rotate)
      used_keys.insert(term->op_code().steps());
  }
  // if (used_keys.size() > keys_threshold)
  //   throw logic_error("number of keys > keys_threshold, rotation needs to be decomposed");

  clog << "used_keys_size: " << used_keys.size() << ", keys_threshold: " << keys_threshold << '\n';

  return used_keys;
}
} // namespace fheco::passes
