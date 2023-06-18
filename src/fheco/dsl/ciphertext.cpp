#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/ops_overloads.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{
Ciphertext::Ciphertext(vector<size_t> shape) : id_{0}, shape_{move(shape)}, idx_{}, example_val_{}
{
  validate_shape(shape_);
}

Ciphertext::Ciphertext(string label, vector<size_t> shape) : Ciphertext(move(shape))
{
  Compiler::active_func()->init_input(*this, move(label));
}

Ciphertext::Ciphertext(string label, PackedVal example_val, vector<size_t> shape) : Ciphertext(move(shape))
{
  Compiler::active_func()->clear_data_evaluator().adjust_packed_val(example_val);
  example_val_ = move(example_val);
  Compiler::active_func()->init_input(*this, move(label));
}

Ciphertext::Ciphertext(string label, integer example_val_slot_min, integer example_val_slot_max, vector<size_t> shape)
  : Ciphertext(move(shape))
{
  example_val_ =
    Compiler::active_func()->clear_data_evaluator().make_rand_packed_val(example_val_slot_min, example_val_slot_max);
  Compiler::active_func()->init_input(*this, move(label));
}

Ciphertext::Ciphertext(const Plaintext &plain)
{
  *this = encrypt(plain);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &other)
{
  if (idx_.size())
    emulate_subscripted_write(*this, other);
  else
  {
    id_ = other.id_;
    shape_ = other.shape_;
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
    shape_ = move(other.shape_);
    example_val_ = move(other.example_val_);
  }
  return *this;
}

const Ciphertext Ciphertext::operator[](size_t idx) const
{
  size_t actual_dim = shape_.size() - idx_.size();
  if (actual_dim == 0)
    throw invalid_argument("subscript on dimension 0");

  if (idx < 0 || idx > shape_[shape_.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  Ciphertext prep = *this;
  prep.idx_.push_back(idx);
  return emulate_subscripted_read(prep);
}

Ciphertext &Ciphertext::operator[](size_t idx)
{
  size_t actual_dim = shape_.size() - idx_.size();
  if (actual_dim == 0)
    throw invalid_argument("subscript on dimension 0");

  if (idx < 0 || idx > shape_[shape_.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  idx_.push_back(idx);
  return *this;
}

const Ciphertext &Ciphertext::set_output(string label) const
{
  Compiler::active_func()->set_output(*this, move(label));
  return *this;
}

void Ciphertext::set_shape(vector<size_t> shape)
{
  shape_ = move(shape);
  validate_shape(shape_);
}
} // namespace fheco
