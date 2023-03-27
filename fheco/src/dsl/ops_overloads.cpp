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
  return operate_binary<Plaintext, Scalar, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::plaintextType);
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

Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Scalar, Ciphertext>(lhs, rhs, ir::OpCode::sub_plain, ir::ciphertextType);
}

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

} // namespace fhecompiler
