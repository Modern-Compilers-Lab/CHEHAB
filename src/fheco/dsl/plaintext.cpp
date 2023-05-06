#include "fheco/dsl/plaintext.hpp"
#include "fheco/dsl/ops_overloads.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{

Plaintext::Plaintext(vector<size_t> shape) : shape_{move(shape)}
{
  validate_shape(shape_);
}

Plaintext::Plaintext(string label, vector<size_t> shape) : Plaintext(move(shape))
{
  Compiler::active_func()->init_input(*this, move(label));
}

Plaintext::Plaintext(string label, PackedVal example_val, vector<size_t> shape) : Plaintext(move(shape))
{
  Compiler::active_func()->init_input(*this, move(label), move(example_val));
}

Plaintext::Plaintext(string label, integer example_val_slot_min, integer example_val_slot_max, vector<size_t> shape)
  : Plaintext(move(shape))
{
  Compiler::active_func()->init_input(*this, move(label), example_val_slot_min, example_val_slot_max);
}

Plaintext::Plaintext(PackedVal packed_val, vector<size_t> shape) : Plaintext(move(shape))
{
  Compiler::active_func()->init_const(*this, move(packed_val));
}

Plaintext::Plaintext(integer scalar_val) : Plaintext(vector<size_t>{})
{
  PackedVal packed_val(Compiler::active_func()->clear_data_evaluator().slot_count(), scalar_val);
  Compiler::active_func()->init_const(*this, move(packed_val));
}

Plaintext &Plaintext::operator=(const Plaintext &other)
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

Plaintext &Plaintext::operator=(Plaintext &&other)
{
  if (idx_.size())
    emulate_subscripted_write(*this, other);
  else
  {
    id_ = other.id_;
    shape_ = other.shape_;
    example_val_ = move(other.example_val_);
  }
  return *this;
}

Plaintext Plaintext::operator[](size_t idx) const
{
  size_t actual_dim = shape_.size() - idx_.size();
  if (actual_dim == 0)
    throw invalid_argument("subscript on dimension 0");

  if (idx < 0 || idx > shape_[shape_.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  Plaintext prep = *this;
  prep.idx_.push_back(idx);
  return emulate_subscripted_read(prep);
}

Plaintext &Plaintext::operator[](size_t idx)
{
  size_t actual_dim = shape_.size() - idx_.size();
  if (actual_dim == 1)
    throw invalid_argument("subscript on dimension 0");

  if (idx < 0 || idx > shape_[shape_.size() - actual_dim] - 1)
    throw invalid_argument("invalid index");

  idx_.push_back(idx);
  return *this;
}

const Plaintext &Plaintext::set_output(string label) const
{
  if (example_val_)
    Compiler::active_func()->set_output(*this, move(label), *example_val_);
  else
    Compiler::active_func()->set_output(*this, move(label));
  return *this;
}

void Plaintext::set_shape(vector<size_t> shape)
{
  shape_ = move(shape);
  validate_shape(shape_);
}
} // namespace fheco
