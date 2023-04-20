#include "common.hpp"

using namespace std;

namespace fhecompiler
{
namespace ir
{
  size_t ConstantValueHash::operator()(const ConstantValue &v) const
  {
    size_t size = vector_size_;
    size_t h = 0;
    visit(
      ir::overloaded{
        [size, &h](const VectorValue &value_var) {
          visit(
            ir::overloaded{[size, &h](const auto &value) {
              for (size_t i = 0; i < size; ++i)
                hash_combine(h, value[i]);
            }},
            value_var);
        },
        [size, &h](const ScalarValue &value_var) {
          visit(
            ir::overloaded{[size, &h](auto value) {
              for (size_t i = 0; i < size; ++i)
                hash_combine(h, value);
            }},
            value_var);
        }},
      v);
    return h;
  }
} // namespace ir
} // namespace fhecompiler
