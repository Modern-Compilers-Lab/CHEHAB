#include "scalar.hpp"
#include "compiler.hpp"
#include <utility>

using namespace std;

namespace fhecompiler
{
size_t Scalar::id_ = 0;

Scalar::Scalar(int64_t data, string tag) : Scalar()
{
  Compiler::get_active().init_const_term(label_, term_type(), move(tag), data);
  // Compiler::init_const(data, example_value_);
}

Scalar::Scalar(int64_t data) : Scalar()
{
  Compiler::get_active().init_const_term(label_, term_type(), label_, data);
  // Compiler::init_const(data, example_value_);
}

Scalar::Scalar(uint64_t data, string tag) : Scalar()
{
  Compiler::get_active().init_const_term(label_, term_type(), move(tag), data);
  // Compiler::init_const(data, example_value_);
}

Scalar::Scalar(uint64_t data) : Scalar()
{
  Compiler::get_active().init_const_term(label_, term_type(), label_, data);
  // Compiler::init_const(data, example_value_);
}

const Scalar &Scalar::tag(string tag) const
{
  Compiler::get_active().tag_term(label_, move(tag));
  return *this;
}
} // namespace fhecompiler
