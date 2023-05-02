#include "fheco/dsl/scalar.hpp"
#include "fheco/dsl/compiler.hpp"

using namespace std;

namespace fheco
{
Scalar::Scalar(integer value)
{
  Compiler::active_func()->init_const(*this, value);
}
} // namespace fheco
