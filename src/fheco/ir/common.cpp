#include "fheco/ir/common.hpp"

using namespace std;

namespace fheco
{
namespace ir
{
  size_t ConstValHash::operator()(const ConstVal &value_var) const
  {
    size_t size = slot_count_;
    size_t h = 0;
    visit(
      ir::overloaded{
        [size, &h](const PackedVal &packed_val) {
          for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
            hash_combine(h, *it);
        },
        [size, &h](ScalarVal scalar_val) {
          for (size_t i = 0; i < size; ++i)
            hash_combine(h, scalar_val);
        }},
      value_var);
    return h;
  }
} // namespace ir
} // namespace fheco
