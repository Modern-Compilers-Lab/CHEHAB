#include "ciphertext.hpp"
#include "compiler.hpp"
#include <utility>

using namespace std;

namespace fhecompiler
{
size_t Ciphertext::id_ = 0;

Ciphertext::Ciphertext(string tag) : Ciphertext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
}

Ciphertext::Ciphertext(string tag, long long min_value, long long max_value) : Ciphertext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
  // Compiler::init_input(label_, tag, min_value, max_value, example_value_);
}

Ciphertext::Ciphertext(string tag, const ir::VectorValue &example_value) : Ciphertext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
  // Compiler::init_input(label_, tag, example_value, example_value_);
}

const Ciphertext &Ciphertext::tag(string tag) const
{
  Compiler::get_active().tag_term(label_, move(tag));
  return *this;
}

const Ciphertext &Ciphertext::set_output(string tag) const
{
  Compiler::get_active().set_term_output(label_, tag);
  return *this;
}
} // namespace fhecompiler
