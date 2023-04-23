#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/compiler.hpp"
#include <utility>

using namespace std;

namespace fheco
{
Ciphertext::Ciphertext(string label)
{
  Compiler::active_func().init_input(*this, move(label));
}

Ciphertext::Ciphertext(string label, PackedVal example_val)
{
  Compiler::active_func().init_input(*this, move(label), move(example_val));
}

Ciphertext::Ciphertext(string label, integer example_val_slot_min, integer example_val_slot_max)
{
  Compiler::active_func().init_input(*this, move(label), example_val_slot_min, example_val_slot_max);
}

const Ciphertext &Ciphertext::set_output(string label) const
{
  if (example_val_)
    Compiler::active_func().set_output(*this, move(label), *example_val_);
  else
    Compiler::active_func().set_output(*this, move(label));
  return *this;
}
} // namespace fheco
