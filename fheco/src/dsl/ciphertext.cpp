#include "ciphertext.hpp"
#include "compiler.hpp"
#include <utility>

using namespace std;

namespace fhecompiler
{
Ciphertext::Ciphertext(string label)
{
  Compiler::active_func().init_input(*this, move(label));
}

// Ciphertext::Ciphertext(string label, const ir::VectorValue &example_value)
// {
//   Compiler::active_func().init_input(*this, move(label), example_value);
// }

// Ciphertext::Ciphertext(string label, int64_t example_value_slot_min, int64_t example_value_slot_max)
// {
//   Compiler::active_func().init_input(*this, move(label), example_value_slot_min, example_value_slot_max);
// }

const Ciphertext &Ciphertext::set_output(string label) const
{
  Compiler::active_func().set_output(*this, move(label));
  return *this;
}
} // namespace fhecompiler
