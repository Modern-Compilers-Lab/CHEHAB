#include "plaintext.hpp"
#include "compiler.hpp"
#include <utility>

using namespace std;

namespace fhecompiler
{

Plaintext::Plaintext(string label)
{
  Compiler::active_func().init_input(*this, move(label));
}

// Plaintext::Plaintext(string label, const ir::VectorValue &example_value)
// {
//   Compiler::active_func().init_input(*this, move(label), example_value);
// }

// Plaintext::Plaintext(string label, int64_t example_value_slot_min, int64_t example_value_slot_max)
// {
//   Compiler::active_func().init_input(*this, move(label), example_value_slot_min, example_value_slot_max);
// }

Plaintext::Plaintext(const vector<int64_t> &value)
{
  Compiler::active_func().init_const(*this, value);
}

Plaintext::Plaintext(const vector<uint64_t> &value)
{
  Compiler::active_func().init_const(*this, value);
}

const Plaintext &Plaintext::set_output(string label) const
{
  Compiler::active_func().set_output(*this, move(label));
  return *this;
}
} // namespace fhecompiler
