#include "scalar.hpp"
#include "compiler.hpp"

using namespace std;

namespace fhecompiler
{
Scalar::Scalar(int64_t value)
{
  Compiler::active_func().init_const(*this, value);
}

// Scalar::Scalar(uint64_t value)
// {
//   Compiler::active_func().init_const(*this, value);
// }
} // namespace fhecompiler
