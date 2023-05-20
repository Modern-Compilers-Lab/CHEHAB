#include "fheco/dsl/common.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/util/common.hpp"
#include <stdexcept>

using namespace std;

namespace fheco
{
void validate_shape(const vector<size_t> &shape)
{
  size_t slot_count = 1;
  for (auto dim_size : shape)
    slot_count = util::mul_safe(slot_count, dim_size);

  if (slot_count > Compiler::active_func()->slot_count())
    throw invalid_argument("shape too large, total number of elements must be <= function slot count");
}
} // namespace fheco
