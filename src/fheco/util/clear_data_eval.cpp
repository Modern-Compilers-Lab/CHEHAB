#include "fheco/util/clear_data_eval.hpp"
#include "fheco/util/common.hpp"
#include <iostream>

using namespace std;

namespace fheco::util
{
void ClearDataEval::adjust_packed_val(PackedVal &packed_val) const
{
  packed_val.resize(slot_count_);
  reduce(packed_val);
}

PackedVal ClearDataEval::make_rand_packed_val(integer slot_min, integer slot_max) const
{
  PackedVal packed_val(slot_count_);
  init_random(packed_val, slot_min, slot_max);
  return packed_val;
}

void ClearDataEval::operate(const ir::OpCode &op_code, const vector<PackedVal> &args, PackedVal &dest) const
{
  if (op_code.arity() != args.size())
    throw invalid_argument("invalid number of args");

  switch (op_code.arity())
  {
  case 1:
    operate_unary(op_code, args[0], dest);
    break;

  case 2:
    operate_binary(op_code, args[0], args[1], dest);
    break;

  default:
    throw logic_error("unhandled clear data evaluation for operations with arity > 2");
  }
}

void ClearDataEval::operate_unary(const ir::OpCode &op_code, const PackedVal &arg, PackedVal &dest) const
{
  switch (op_code.type())
  {
  case ir::OpCode::Type::negate:
    negate(arg, dest);
    break;

  case ir::OpCode::Type::rotate:
    rotate(arg, op_code.steps(), dest);
    break;

  case ir::OpCode::Type::square:
    mul(arg, arg, dest);
    break;

  default:
    throw logic_error("unhandled clear evaluation for unary operation");
  }
}

void ClearDataEval::operate_binary(
  const ir::OpCode &op_code, const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const
{
  switch (op_code.type())
  {
  case ir::OpCode::Type::add:
    add(arg1, arg2, dest);
    break;

  case ir::OpCode::Type::sub:
    sub(arg1, arg2, dest);
    break;

  case ir::OpCode::Type::mul:
    mul(arg1, arg2, dest);
    break;

  default:
    throw logic_error("unhandled clear evaluation for binary operation");
  }
}

void ClearDataEval::reduce(PackedVal &packed_val) const
{
  if (overflow_warnings_ && delayed_reduction_)
  {
    if (signedness_)
    {
      for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
      {
        if (*it > modulus_)
          cerr << "detected signed overflow\n";
        else if (*it > modulus_ >> 1 && *it <= modulus_)
          cerr << "possible signed overflow later in the homomorphic evaluation\n";
        else if (*it < -modulus_ >> 1 && *it >= -modulus_)
          cerr << "possible signed underflow later in the homomorphic evaluation\n";
        else if (*it < -modulus_)
          cerr << "detected signed underflow\n";
      }
    }
    // !signedness_
    else
    {
      for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
      {
        if (*it > modulus_ || *it < 0)
          cerr << "possible manipulation of multiple equivalent values as different\n";
      }
    }
  }
  // !delayed_reduction_
  else if (!delayed_reduction_)
  {
    for (auto it = packed_val.begin(); it != packed_val.end(); ++it)
    {
      if (overflow_warnings_ && signedness_)
      {
        if (*it > modulus_ >> 1)
          cerr << "detected signed overflow\n";
        else if (*it < -modulus_ >> 1)
          cerr << "detected signed underflow\n";
      }
      if (*it > modulus_)
        *it %= modulus_;
      else if (*it < -modulus_ >> 1 || (!signedness_ && *it < 0))
      {
        *it %= modulus_;
        *it += modulus_;
      }
    }
  }
}

void ClearDataEval::add(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const
{
  dest.resize(slot_count_);
  for (size_t i = 0; i < slot_count_; ++i)
    dest[i] = add_safe(arg1[i], arg2[i]);

  reduce(dest);
}

void ClearDataEval::sub(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const
{
  dest.resize(slot_count_);
  for (size_t i = 0; i < slot_count_; ++i)
    dest[i] = sub_safe(arg1[i], arg2[i]);

  reduce(dest);
}

void ClearDataEval::negate(const PackedVal &arg, PackedVal &dest) const
{
  dest.resize(slot_count_);
  for (size_t i = 0; i < slot_count_; ++i)
    dest[i] = -arg[i];

  reduce(dest);
}

void ClearDataEval::rotate(const PackedVal &arg, int steps, PackedVal &dest) const
{
  dest.resize(slot_count_);
  int64_t slot_count = static_cast<int64_t>(slot_count_);
  steps %= slot_count;
  if (steps < 0)
    steps += slot_count;
  for (size_t i = 0; i < slot_count_; ++i)
    dest[i] = arg[(i + steps) % slot_count_];
}

void ClearDataEval::mul(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const
{
  dest.resize(slot_count_);
  for (size_t i = 0; i < slot_count_; ++i)
    dest[i] = mul_safe(arg1[i], arg2[i]);

  reduce(dest);
}
} // namespace fheco::util