#include "ops_overloads.hpp"

namespace fhecompiler
{

// addition
Ciphertext &operator+=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
  return lhs;
}

Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs)
{
  return rhs + lhs;
}

Plaintext operator+(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
}

Plaintext &operator+=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
  return lhs;
}

Plaintext operator+(const Scalar &rhs, const Plaintext &lhs)
{
  return lhs + rhs;
}

Ciphertext operator+(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext &operator+=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
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
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Plaintext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext &operator-=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
  return lhs;
}

Plaintext operator-(const Plaintext &lhs, const Scalar &rhs)
{
  return operate_binary<Plaintext, Plaintext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
}

Plaintext &operator-=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
  return lhs;
}

Plaintext operator-(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
  return lhs;
}

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext &operator*=(Ciphertext &lhs, const Plaintext &rhs)
{
  compound_operate<Ciphertext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
  return lhs;
}

Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs)
{
  return rhs * lhs;
}

Ciphertext operator*(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext operator*(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext &operator*=(Ciphertext &lhs, const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
  return lhs;
}

Plaintext operator*(const Scalar &lhs, const Plaintext &rhs)
{
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
}

Plaintext &operator*=(Plaintext &lhs, const Scalar &rhs)
{
  compound_operate<Plaintext, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
  return lhs;
}

Plaintext operator*(const Plaintext &lhs, const Scalar &rhs)
{
  return rhs * lhs;
}

} // namespace fhecompiler
