#include "fheco/dsl/scalar.hpp"
#include "fheco/dsl/compiler.hpp"

using namespace std;

namespace fheco
{
Scalar::Scalar(int64_t value)
{
  Compiler::active_func().init_const(*this, value);
}

// Scalar::Scalar(uint64_t value)
// {
//   Compiler::active_func().init_const(*this, value);
// }
} // namespace fheco
