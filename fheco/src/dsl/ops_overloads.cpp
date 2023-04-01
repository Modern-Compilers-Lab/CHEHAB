#include "ops_overloads.hpp"
#include "datatypes_util.hpp"
#include <cstddef>
#include <vector>

namespace fhecompiler
{

// addition
Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext operator+(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Plaintext, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
}

Plaintext operator+(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
}

Ciphertext operator+(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Plaintext operator+(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
}

Scalar operator+(const Scalar &lhs, const Scalar &rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::scalar);
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

Ciphertext &operator+=(Ciphertext &lhs, const Scalar &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

Plaintext &operator+=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

Plaintext &operator+=(Plaintext &lhs, const Scalar &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

Scalar &operator+=(Scalar &lhs, const Scalar &rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

// subtraction
Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Plaintext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
}

Plaintext operator-(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
}

Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Plaintext operator-(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
}

Scalar operator-(const Scalar &lhs, const Scalar &rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::scalar);
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

Ciphertext &operator-=(Ciphertext &lhs, const Scalar &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Plaintext &operator-=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Plaintext &operator-=(Plaintext &lhs, const Scalar &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Scalar &operator-=(Scalar &lhs, const Scalar &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext operator*(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Plaintext, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
}

Plaintext operator*(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
}

Ciphertext operator*(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Plaintext operator*(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
}

Scalar operator*(const Scalar &lhs, const Scalar &rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::scalar);
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

Ciphertext &operator*=(Ciphertext &lhs, const Scalar &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

Plaintext &operator*=(Plaintext &lhs, const Plaintext &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

Plaintext &operator*=(Plaintext &lhs, const Scalar &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

Scalar &operator*=(Scalar &lhs, const Scalar &rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

// negation
Ciphertext operator-(const Ciphertext &lhs)
{
  return operate_unary<Ciphertext, Ciphertext>(lhs, ir::OpCode::negate, ir::TermType::ciphertext);
}

Plaintext operator-(const Plaintext &lhs)
{
  return operate_unary<Plaintext, Plaintext>(lhs, ir::OpCode::negate, ir::TermType::plaintext);
}

Scalar operator-(const Scalar &lhs)
{
  return operate_unary<Scalar, Scalar>(lhs, ir::OpCode::negate, ir::TermType::scalar);
}

// rotation
Ciphertext rotate(const Ciphertext &lhs, int steps)
{
  size_t vector_size = Compiler::get_active()->get_vector_size();
  steps %= vector_size;
  if (steps < 0)
    steps += vector_size;
  Ciphertext result =
    operate_with_raw<Ciphertext>(lhs, std::to_string(steps), ir::OpCode::rotate, ir::TermType::ciphertext);
  Compiler::operate_rotate(lhs.example_value(), steps, result.example_value());
  return result;
}

Plaintext rotate(const Plaintext &lhs, int steps)
{
  size_t vector_size = Compiler::get_active()->get_vector_size();
  steps %= vector_size;
  if (steps < 0)
    steps += vector_size;
  Plaintext result =
    operate_with_raw<Plaintext>(lhs, std::to_string(steps), ir::OpCode::rotate, ir::TermType::plaintext);
  Compiler::operate_rotate(lhs.example_value(), steps, result.example_value());
  return result;
}

Ciphertext operator<<(const Ciphertext &lhs, int steps)
{
  return rotate(lhs, steps);
}

Ciphertext operator>>(const Ciphertext &lhs, int steps)
{
  return rotate(lhs, -steps);
}

Plaintext operator<<(const Plaintext &lhs, int steps)
{
  return rotate(lhs, steps);
}

Plaintext operator>>(const Plaintext &lhs, int steps)
{
  return rotate(lhs, -steps);
}

// rotation assignement
Ciphertext &operator<<=(Ciphertext &lhs, int steps)
{
  lhs = lhs << steps;
  return lhs;
}

Ciphertext &operator>>=(Ciphertext &lhs, int steps)
{
  lhs = lhs >> steps;
  return lhs;
}

Plaintext &operator<<=(Plaintext &lhs, int steps)
{
  lhs = lhs << steps;
  return lhs;
}

Plaintext &operator>>=(Plaintext &lhs, int steps)
{
  lhs = lhs >> steps;
  return lhs;
}

// square, expo, multiply many, add_many, reduce*

Ciphertext reduce_add(const Ciphertext &encrypted)
{
  Ciphertext result = encrypted;
  std::size_t step = Compiler::get_active()->get_vector_size() >> 1;
  while (step > 0)
  {
    result += result << step;
    step >>= 1;
  }
  return result;
}

Ciphertext add_many(const std::vector<Ciphertext> &encrypteds)
{
  std::vector<Ciphertext> sum_vec;
  for (size_t i = 0; i < encrypteds.size() - 1; i += 2)
    sum_vec.push_back(encrypteds[i] + encrypteds[i + 1]);

  if (encrypteds.size() & 1)
    sum_vec.push_back(encrypteds.back());

  for (size_t i = 0; i < sum_vec.size() - 1; i += 2)
    sum_vec.push_back(sum_vec[i] + sum_vec[i + 1]);

  return sum_vec.back();
}

} // namespace fhecompiler
