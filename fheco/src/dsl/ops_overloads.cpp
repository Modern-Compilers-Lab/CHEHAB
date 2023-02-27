#include "ops_overloads.hpp"

namespace fhecompiler
{

// addition
Ciphertext &operator+=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
  return lhs;
}

Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
}

Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs)
{
  return rhs + lhs;
}

Plaintext operator+(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::add, ir::plaintextType);
}

Plaintext &operator+=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::add, ir::plaintextType);
  return lhs;
}

Plaintext operator+(const Scalar &rhs, const Plaintext &lhs)
{
  return lhs + rhs;
}

Ciphertext operator+(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
}

Ciphertext &operator+=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
  return lhs;
}

Ciphertext operator+(const Scalar &lhs, const Ciphertext &rhs)
{
  // return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
  return rhs + lhs;
}

// subtraction
Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Plaintext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext &operator-=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
  return lhs;
}

Plaintext operator-(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::plaintextType);
}

Plaintext &operator-=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::plaintextType);
  return lhs;
}

Plaintext operator-(const Scalar &lhs, const Plaintext &rhs)
{
  return rhs - lhs;
}

Ciphertext operator-(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext operator-=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
  return lhs;
}

/*
Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  return datatype::operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::sub_plain, ir::ciphertextType);
}
*/

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
}

Ciphertext &operator*=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
  return lhs;
}

Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs)
{
  return rhs * lhs;
}

Ciphertext operator*(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
}

Ciphertext operator*(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
}

Ciphertext &operator*=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
  return lhs;
}

Plaintext operator*(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::plaintextType);
}

Plaintext &operator*=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::plaintextType);
  return lhs;
}

Plaintext operator*(const Plaintext &lhs, const Scalar &rhs)
{
  return rhs * lhs;
}

Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
}

Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext &rhs)
{
  return operate_unary<Ciphertext, Ciphertext>(rhs, ir::OpCode::negate, ir::ciphertextType);
}

Ciphertext exponentiate(const Ciphertext &lhs, uint32_t exponent)
{
  return operate_with_raw<Ciphertext>(lhs, std::to_string(exponent), ir::OpCode::exponentiate, ir::ciphertextType);
}

Ciphertext square(const Ciphertext &lhs)
{
  return operate_unary<Ciphertext, Ciphertext>(lhs, ir::OpCode::square, ir::ciphertextType);
}

Ciphertext operator<<(const Ciphertext &lhs, int steps)
{
  return rotate(lhs, steps);
}

Ciphertext operator>>(const Ciphertext &lhs, int steps)
{
  return rotate(lhs, -steps);
}

Ciphertext &Ciphertext::operator<<=(int steps)
{
  this->rotate(steps);
  return *this;
}

Ciphertext &Ciphertext::operator>>=(int steps)
{
  this->rotate(-steps);
  return *this;
}

Ciphertext rotate(const Ciphertext &lhs, int steps)
{

  ir::OpCode opcode;

  if (program->get_targeted_backend() == Backend::SEAL && program->get_encryption_scheme() != Scheme::ckks)
  {
    opcode = ir::OpCode::rotate_rows;
  }
  else
    opcode = ir::OpCode::rotate;

  return operate_with_raw<Ciphertext>(lhs, std::to_string(steps), opcode, ir::ciphertextType);
}

Ciphertext rotate_rows(const Ciphertext &lhs, int steps)
{
  ir::OpCode opcode;

  if (program->get_targeted_backend() != Backend::SEAL)
  {
    opcode = ir::OpCode::rotate;
  }
  else
    opcode = ir::OpCode::rotate_rows;

  return operate_with_raw<Ciphertext>(lhs, std::to_string(steps), opcode, ir::ciphertextType);
}

Ciphertext rotate_columns(const Ciphertext &lhs)
{
  if (program->get_targeted_backend() != Backend::SEAL)
    return lhs;

  return operate_unary<Ciphertext, Ciphertext>(lhs, ir::OpCode::rotate_columns, ir::ciphertextType);
}

} // namespace fhecompiler
