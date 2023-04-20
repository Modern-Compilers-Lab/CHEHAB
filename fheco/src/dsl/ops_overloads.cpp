#include "ops_overloads.hpp"
#include "compiler.hpp"
#include "op_code.hpp"
#include <cstddef>
#include <vector>

using namespace std;

namespace fhecompiler
{
// addition
Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Ciphertext operator+(const Ciphertext &lhs, const Scalar &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Plaintext operator+(const Plaintext &lhs, const Scalar &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Ciphertext operator+(const Scalar &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Plaintext operator+(const Scalar &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
}

Scalar operator+(const Scalar &lhs, const Scalar &rhs)
{
  Scalar destination{};
  Compiler::active_func().operate_binary(ir::OpCode::add, lhs, rhs, destination);
  return destination;
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
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Ciphertext operator-(const Ciphertext &lhs, const Scalar &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Plaintext operator-(const Plaintext &lhs, const Scalar &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Plaintext operator-(const Scalar &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
}

Scalar operator-(const Scalar &lhs, const Scalar &rhs)
{
  Scalar destination{};
  Compiler::active_func().operate_binary(ir::OpCode::sub, lhs, rhs, destination);
  return destination;
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
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Ciphertext operator*(const Ciphertext &lhs, const Scalar &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Plaintext operator*(const Plaintext &lhs, const Scalar &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Ciphertext operator*(const Scalar &lhs, const Ciphertext &rhs)
{
  Ciphertext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Plaintext operator*(const Scalar &lhs, const Plaintext &rhs)
{
  Plaintext destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
}

Scalar operator*(const Scalar &lhs, const Scalar &rhs)
{
  Scalar destination{};
  Compiler::active_func().operate_binary(ir::OpCode::mul, lhs, rhs, destination);
  return destination;
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
Ciphertext operator-(const Ciphertext &arg)
{
  Ciphertext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::negate, arg, destination);
  return destination;
}

Plaintext operator-(const Plaintext &arg)
{
  Plaintext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::negate, arg, destination);
  return destination;
}

Scalar operator-(const Scalar &arg)
{
  Scalar destination{};
  Compiler::active_func().operate_unary(ir::OpCode::negate, arg, destination);
  return destination;
}

// rotation
Ciphertext rotate(const Ciphertext &arg, int steps)
{
  size_t vector_size = Compiler::active_func().vector_size();
  // this should work since vector_size is a power of 2
  steps %= vector_size;
  Ciphertext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::rotate(steps), arg, destination);
  return destination;
}

Plaintext rotate(const Plaintext &arg, int steps)
{
  size_t vector_size = Compiler::active_func().vector_size();
  // this should work since vector_size is a power of 2
  steps %= vector_size;
  Plaintext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::rotate(steps), arg, destination);
  return destination;
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

// encryption
Ciphertext encrypt(const Plaintext &arg)
{
  Ciphertext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::encrypt, arg, destination);
  return destination;
}

Ciphertext encrypt(const Scalar &arg)
{
  Ciphertext destination{};
  Compiler::active_func().operate_unary(ir::OpCode::encrypt, arg, destination);
  return destination;
}

// square, expo, multiply many, add_many, reduce*
Ciphertext square(const Ciphertext &encrypted)
{
  return encrypted * encrypted;
}

Ciphertext reduce_add(const Ciphertext &encrypted)
{
  Ciphertext result = encrypted;
  size_t steps = Compiler::active_func().vector_size() >> 1;
  while (steps > 0)
  {
    result += result << steps;
    steps >>= 1;
  }
  return result;
}

Ciphertext add_many(const vector<Ciphertext> &encrypteds)
{
  vector<Ciphertext> sum_vec;
  for (size_t i = 0; i < encrypteds.size() - 1; i += 2)
    sum_vec.push_back(encrypteds[i] + encrypteds[i + 1]);

  if (encrypteds.size() & 1)
    sum_vec.push_back(encrypteds.back());

  for (size_t i = 0; i < sum_vec.size() - 1; i += 2)
    sum_vec.push_back(sum_vec[i] + sum_vec[i + 1]);

  return sum_vec.back();
}
} // namespace fhecompiler
