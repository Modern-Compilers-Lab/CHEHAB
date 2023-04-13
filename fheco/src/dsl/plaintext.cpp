#include "plaintext.hpp"
#include "compiler.hpp"
#include <utility>

using namespace std;

namespace fhecompiler
{
size_t Plaintext::id_ = 0;

Plaintext::Plaintext(string tag) : Plaintext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
}

Plaintext::Plaintext(string tag, long long slot_min_value, long long slot_max_value) : Plaintext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
  // Compiler::init_input(label_, tag, example_value, example_value_);
}

Plaintext::Plaintext(string tag, const ir::VectorValue &example_value) : Plaintext()
{
  Compiler::get_active().init_input_term(label_, term_type(), move(tag));
  // Compiler::init_input(label_, tag, example_value, example_value_);
}

Plaintext::Plaintext(const vector<int64_t> &data, string tag) : Plaintext()
{
  Compiler::get_active().init_const_term(label_, term_type(), move(tag), data);
}

Plaintext::Plaintext(const vector<int64_t> &data) : Plaintext()
{
  Compiler::get_active().init_const_term(label_, term_type(), "const_" + label_, data);
}

Plaintext::Plaintext(const vector<int> &data, string tag) : Plaintext()
{
  Compiler::get_active().init_const_term(label_, term_type(), move(tag), vector<int64_t>(data.begin(), data.end()));
}

Plaintext::Plaintext(const vector<int> &data) : Plaintext()
{
  Compiler::get_active().init_const_term(
    label_, term_type(), "const_" + label_, vector<int64_t>(data.begin(), data.end()));
}

Plaintext::Plaintext(const vector<uint64_t> &data, string tag) : Plaintext()
{
  Compiler::get_active().init_const_term(label_, term_type(), move(tag), data);
}

Plaintext::Plaintext(const vector<uint64_t> &data) : Plaintext()
{
  Compiler::get_active().init_const_term(label_, term_type(), "const_" + label_, data);
}

const Plaintext &Plaintext::tag(string tag) const
{
  Compiler::get_active().tag_term(label_, move(tag));
  return *this;
}

const Plaintext &Plaintext::set_output(string tag) const
{
  Compiler::get_active().set_term_output(label_, tag);
  return *this;
}
} // namespace fhecompiler
