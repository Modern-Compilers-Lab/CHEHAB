#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/ops_overloads.hpp"
#include "fheco/dsl/plaintext.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{
Ciphertext::Ciphertext(string label) : Ciphertext{}
{
  Compiler::active_func()->init_input(*this, move(label));
}

Ciphertext::Ciphertext(string label, PackedVal example_val) : Ciphertext{}
{
  Compiler::active_func()->init_input(*this, move(label), move(example_val));
}

Ciphertext::Ciphertext(string label, integer example_val_slot_min, integer example_val_slot_max) : Ciphertext{}
{
  Compiler::active_func()->init_input(*this, move(label), example_val_slot_min, example_val_slot_max);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &other)
{
  if (idx_.size())
    emulate_subscripted_write(*this, other);
  else
  {
    id_ = other.id_;
    dim_ = other.dim_;
    example_val_ = other.example_val_;
  }
  return *this;
}

Ciphertext &Ciphertext::operator=(Ciphertext &&other)
{
  if (idx_.size())
    emulate_subscripted_write(*this, other);
  else
  {
    id_ = other.id_;
    dim_ = other.dim_;
    example_val_ = move(other.example_val_);
  }
  return *this;
}

const Ciphertext Ciphertext::operator[](size_t idx) const
{
  size_t actual_dim = dim_ - idx_.size();
  if (actual_dim < 1)
    throw invalid_argument("subscript on dimension 0");

  const auto &shape = Compiler::active_func()->shape();
  if (idx < 0 || idx > shape[shape.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  Ciphertext prep = *this;
  prep.idx_.push_back(idx);
  return emulate_subscripted_read(prep);
}

Ciphertext &Ciphertext::operator[](size_t idx)
{
  size_t actual_dim = dim_ - idx_.size();
  if (Compiler::active_func()->is_valid_term_id(id_))
  {
    if (actual_dim < 1)
      throw invalid_argument("subscript on dimension 0");
  }
  else
    dim_ = Compiler::active_func()->shape().size();

  const auto &shape = Compiler::active_func()->shape();
  if (idx < 0 || idx > shape[shape.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  idx_.push_back(idx);
  return *this;
}

const Ciphertext &Ciphertext::set_output(string label) const
{
  if (example_val_)
    Compiler::active_func()->set_output(*this, move(label), *example_val_);
  else
    Compiler::active_func()->set_output(*this, move(label));
  return *this;
}
} // namespace fheco
