#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/ops_overloads.hpp"
#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility> 
#include <vector>
# include <iostream>
#include <iostream>
#include <cstdlib>  

using namespace std;

namespace fheco
{
// addition
Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::add, lhs, rhs, dest);
  return dest;
}

Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::add, lhs, rhs, dest);
  return dest;
}

Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::add, lhs, rhs, dest);
  return dest;
}

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Plaintext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::add, lhs, rhs, dest);
  return dest;
}

// addition assignement
Ciphertext &operator+=(Ciphertext &lhs, const Ciphertext &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

Ciphertext &operator+=(Ciphertext &lhs, const Plaintext &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

Plaintext &operator+=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

// subtraction
Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::sub, lhs, rhs, dest);
  return dest;
}

Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::sub, lhs, rhs, dest);
  return dest;
}

Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::sub, lhs, rhs, dest);
  return dest;
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Plaintext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::sub, lhs, rhs, dest);
  return dest;
}

// subtraction assignement
Ciphertext &operator-=(Ciphertext &lhs, const Ciphertext &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Ciphertext &operator-=(Ciphertext &lhs, const Plaintext &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Plaintext &operator-=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");
  
  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::mul, lhs, rhs, dest);
  return dest;
}
/********************************************************************/
Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");
  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::mul, lhs, rhs, dest);
  return dest;
}
/*********************************/
Ciphertext operator*(const Ciphertext* lhs, const Plaintext &rhs)
{
  if ((*lhs).idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");
  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::mul, *lhs, rhs, dest);
  return dest;
}

/********************************************************************/
Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::mul, lhs, rhs, dest);
  return dest;
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  if (lhs.idx().size() || rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Plaintext dest{};
  Compiler::active_func()->operate_binary(ir::OpCode::mul, lhs, rhs, dest);
  return dest;
}

// multiplication assignement
Ciphertext &operator*=(Ciphertext &lhs, const Ciphertext &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

Ciphertext &operator*=(Ciphertext &lhs, const Plaintext &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

Plaintext &operator*=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

// negation
Ciphertext operator-(const Ciphertext &arg)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::negate, arg, dest);
  return dest;
}

Plaintext operator-(const Plaintext &arg)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Plaintext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::negate, arg, dest);
  return dest;
}

// rotation  
Ciphertext rotate(const Ciphertext &arg, int steps)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  auto signed_slot_count = static_cast<int64_t>(Compiler::active_func()->slot_count());
  //int real_slot_count = 2048 ;
  if (steps >= signed_slot_count || steps < 0)
  {
    steps %= signed_slot_count;
    /*std::cout<<"rotations in ops_overloads :"<<steps<<" \n";
    if (steps < 0) 
      steps += real_slot_count;*/
  }
  
  Ciphertext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::rotate(steps), arg, dest);
  return dest;
}

Plaintext rotate(const Plaintext &arg, int steps)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  auto signed_slot_count = static_cast<int64_t>(Compiler::active_func()->slot_count());
  if (steps >= signed_slot_count || steps < 0)
  {
    steps %= signed_slot_count;
    if (steps < 0)
      steps += signed_slot_count;
  }
  Plaintext dest{}; 
  Compiler::active_func()->operate_unary(ir::OpCode::rotate(steps), arg, dest);
  return dest;
}

Ciphertext operator<<(const Ciphertext &arg, int steps)
{
  return rotate(arg, steps);
}

Ciphertext operator>>(const Ciphertext &arg, int steps)
{
  return rotate(arg, -steps);
}

Plaintext operator<<(const Plaintext &arg, int steps)
{
  return rotate(arg, steps);
}

Plaintext operator>>(const Plaintext &arg, int steps)
{
  return rotate(arg, -steps);
}

// rotation assignement
Ciphertext &operator<<=(Ciphertext &arg, int steps)
{
  arg = arg << steps;
  return arg;
}

Ciphertext &operator>>=(Ciphertext &arg, int steps)
{
  arg = arg >> steps;
  return arg;
}

Plaintext &operator<<=(Plaintext &arg, int steps)
{
  arg = arg << steps;
  return arg;
}

Plaintext &operator>>=(Plaintext &arg, int steps)
{
  arg = arg >> steps;
  return arg;
}
/***************************************************************************************/
Ciphertext SumVec(const Ciphertext &arg, int size)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");
  auto signed_slot_count = static_cast<int64_t>(Compiler::active_func()->slot_count());
  if (size > signed_slot_count || size < 0)
  {
    size %= signed_slot_count;
    if (size < 0)
      size += signed_slot_count;
  }
  Ciphertext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::SumVec(size), arg, dest);
  return dest;
}
/***************************************************************************************/
// encryption
Ciphertext encrypt(const Plaintext &arg)
{
  if (arg.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  Ciphertext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::encrypt, arg, dest);
  return dest;
}

// subscript helper
pair<int, int> compute_subscripted_range(const Ciphertext &arg)
{
  if (arg.idx().empty())
    throw invalid_argument("subscripted has an empty index");

  int dim = arg.shape().size();
  size_t start = 0;
  for (auto axis_idx : arg.idx())
  {
    size_t lower_dims_slot_count = 1;
    for (size_t i = arg.shape().size() - dim + 1; i < arg.shape().size(); ++i)
      lower_dims_slot_count *= arg.shape()[i];
    start += axis_idx * lower_dims_slot_count;
    --dim;
  }
  size_t end = start + ((dim > 0) ? arg.shape()[arg.shape().size() - dim] : 1);
  return {start, end};
}

pair<int, int> compute_subscripted_range(const Plaintext &arg)
{
  if (arg.idx().empty())
    throw invalid_argument("subscripted has an empty index");

  int dim = arg.shape().size();
  size_t start = 0;
  for (auto axis_idx : arg.idx())
  {
    size_t lower_dims_slot_count = 1;
    for (size_t i = arg.shape().size() - dim + 1; i < arg.shape().size(); ++i)
      lower_dims_slot_count *= arg.shape()[i];
    start += axis_idx * lower_dims_slot_count;
    --dim;
  }
  size_t end = start + ((dim > 0) ? arg.shape()[arg.shape().size() - dim] : 1);
  return {start, end};
}

// subscripted_read
Ciphertext emulate_subscripted_read(const Ciphertext &arg)
{
  auto [start, end] = compute_subscripted_range(arg);
  PackedVal mask(Compiler::active_func()->slot_count(), 0);

  for (size_t i = start; i < end; ++i)
    mask[i] = 1;
  Ciphertext resolved = arg;
  resolved.idx_.clear();
  Plaintext plain_mask(mask);
  plain_mask.shape_ = arg.shape();
  Ciphertext result = (resolved * plain_mask) << start;
  result.shape_.erase(result.shape_.begin(), result.shape_.begin() + arg.idx_.size());
  return result;
}

Plaintext emulate_subscripted_read(const Plaintext &arg)
{
  auto [start, end] = compute_subscripted_range(arg);
  PackedVal mask(Compiler::active_func()->slot_count(), 0);

  for (size_t i = start; i < end; ++i)
    mask[i] = 1;
  Plaintext resolved = arg;
  resolved.idx_.clear();
  Plaintext plain_mask(mask);
  plain_mask.shape_ = arg.shape();
  Plaintext result = (resolved * plain_mask) << start;
  result.shape_.erase(result.shape_.begin(), result.shape_.begin() + arg.idx_.size());
  return result;
}

void emulate_subscripted_write(Ciphertext &lhs, const Ciphertext &rhs)
{
  if (rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  auto [start, end] = compute_subscripted_range(lhs);
  vector<size_t> actual_shape = lhs.shape();
  lhs.shape_.erase(lhs.shape_.begin(), lhs.shape_.begin() + lhs.idx_.size());
  lhs.idx_.clear();
  if (Compiler::active_func()->data_flow().is_valid_term_id(lhs.id()))
  {
    PackedVal mask(Compiler::active_func()->slot_count(), 1);
    for (size_t i = start; i < end; ++i)
      mask[i] = 0;
    Plaintext plain_mask(mask);
    plain_mask.shape_ = lhs.shape();
    lhs = lhs * plain_mask + (rhs >> start);
    lhs.shape_ = actual_shape;
  }
  else
  {
    lhs = rhs >> start;
    lhs.shape_ = actual_shape;
  }
}

void emulate_subscripted_write(Plaintext &lhs, const Plaintext &rhs)
{
  if (rhs.idx().size())
    throw invalid_argument("subscript read must be performed on const variables");

  auto [start, end] = compute_subscripted_range(lhs);
  vector<size_t> actual_shape = lhs.shape();
  lhs.shape_.erase(lhs.shape_.begin(), lhs.shape_.begin() + lhs.idx_.size());
  lhs.idx_.clear();
  if (Compiler::active_func()->data_flow().is_valid_term_id(lhs.id()))
  {
    PackedVal mask(Compiler::active_func()->slot_count(), 1);
    for (size_t i = start; i < end; ++i)
      mask[i] = 0;
    Plaintext plain_mask(mask);
    plain_mask.shape_ = lhs.shape();
    lhs = lhs * plain_mask + (rhs >> start);
    lhs.shape_ = actual_shape;
  }
  else
  {
    lhs = rhs >> start;
    lhs.shape_ = actual_shape;
  }
}

// square
Ciphertext square(const Ciphertext &arg)
{
  Ciphertext dest{};
  Compiler::active_func()->operate_unary(ir::OpCode::square, arg, dest);
  return dest;
}

Plaintext square(const Plaintext &arg)
{
  return arg * arg;
}

// add_many
Ciphertext add_many(const vector<Ciphertext> &args)
{
  if (args.empty())
    throw invalid_argument("empty args vector");

  vector<Ciphertext> sum_vec;
  for (size_t i = 0; i < args.size() - 1; i += 2)
    sum_vec.push_back(args[i] + args[i + 1]);

  if (args.size() & 1)
    sum_vec.push_back(args.back());

  for (size_t i = 0; i < sum_vec.size() - 1; i += 2)
    sum_vec.push_back(sum_vec[i] + sum_vec[i + 1]);

  return sum_vec.back();
}

Plaintext add_many(const vector<Plaintext> &args)
{
  if (args.empty())
    throw invalid_argument("empty args vector");

  Plaintext result(0);
  for (size_t i = 0; i < args.size(); ++i)
    result += args[i];
  return result;
}

// mul_many
Ciphertext mul_many(const vector<Ciphertext> &args)
{
  if (args.empty())
    throw invalid_argument("empty args vector");

  vector<Ciphertext> sum_vec;
  for (size_t i = 0; i < args.size() - 1; i += 2)
    sum_vec.push_back(args[i] * args[i + 1]);

  if (args.size() & 1)
    sum_vec.push_back(args.back());

  for (size_t i = 0; i < sum_vec.size() - 1; i += 2)
    sum_vec.push_back(sum_vec[i] * sum_vec[i + 1]);

  return sum_vec.back();
}

Plaintext mul_many(const vector<Plaintext> &args)
{
  if (args.empty())
    throw invalid_argument("empty args vector");

  Plaintext result(1);
  for (size_t i = 0; i < args.size(); ++i)
    result *= args[i];
  return result;
}

// exponentiate
Ciphertext exponentiate(const Ciphertext &arg, uint64_t exponent)
{
  if (exponent == 0)
    return encrypt(Plaintext(1));

  vector<Ciphertext> args(static_cast<size_t>(exponent), arg);
  return mul_many(args);
}
Plaintext exponentiate(const Plaintext &arg, uint64_t exponent)
{
  if (exponent == 0)
    return Plaintext(1);

  vector<Plaintext> args(static_cast<size_t>(exponent), arg);
  return mul_many(args);
}

// reduce_add
Ciphertext reduce_add(const Ciphertext &arg)
{
  Ciphertext result = arg;
  size_t steps = Compiler::active_func()->slot_count() >> 1;
  while (steps > 0)
  {
    result += result << steps;
    steps >>= 1;
  }
  return result;
}

Plaintext reduce_add(const Plaintext &arg)
{
  Plaintext result(0);
  for (size_t i = 0; i < Compiler::active_func()->slot_count(); ++i)
    result += arg << i;
  return result;
}

// reduce_mul
Ciphertext reduce_mul(const Ciphertext &arg)
{
  Ciphertext result = arg;
  size_t steps = Compiler::active_func()->slot_count() >> 1;
  while (steps > 0)
  {
    result *= result << steps;
    steps >>= 1;
  }
  return result;
}

Plaintext reduce_mul(const Plaintext &arg)
{
  Plaintext result(1);
  for (size_t i = 0; i < Compiler::active_func()->slot_count(); ++i)
    result *= arg << i;
  return result;
}
} // namespace fheco