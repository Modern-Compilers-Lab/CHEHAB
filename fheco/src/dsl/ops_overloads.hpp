#pragma once

#include "ciphertext.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"

namespace fhecompiler
{

// addition
Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator+(const Ciphertext &lhs, const Scalar &rhs);
Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs);
Plaintext operator+(const Plaintext &lhs, const Scalar &rhs);
Ciphertext operator+(const Scalar &lhs, const Ciphertext &rhs);
Plaintext operator+(const Scalar &lhs, const Plaintext &rhs);
Scalar operator+(const Scalar &lhs, const Scalar &rhs);

// addition assignement
Ciphertext &operator+=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator+=(Ciphertext &lhs, const Plaintext &rhs);
Ciphertext &operator+=(Ciphertext &lhs, const Scalar &rhs);
Plaintext &operator+=(Plaintext &lhs, const Plaintext &rhs);
Plaintext &operator+=(Plaintext &lhs, const Scalar &rhs);
Scalar &operator+=(Scalar &lhs, const Scalar &rhs);

// subtraction
Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator-(const Ciphertext &lhs, const Scalar &rhs);
Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs);
Plaintext operator-(const Plaintext &lhs, const Scalar &rhs);
Ciphertext operator-(const Scalar &lhs, const Ciphertext &rhs);
Plaintext operator-(const Scalar &lhs, const Plaintext &rhs);
Scalar operator-(const Scalar &lhs, const Scalar &rhs);

// subtraction assignement
Ciphertext &operator-=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator-=(Ciphertext &lhs, const Plaintext &rhs);
Ciphertext &operator-=(Ciphertext &lhs, const Scalar &rhs);
Plaintext &operator-=(Plaintext &lhs, const Plaintext &rhs);
Plaintext &operator-=(Plaintext &lhs, const Scalar &rhs);
Scalar &operator-=(Scalar &lhs, const Scalar &rhs);

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator*(const Ciphertext &lhs, const Scalar &rhs);
Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs);
Plaintext operator*(const Plaintext &lhs, const Scalar &rhs);
Ciphertext operator*(const Scalar &lhs, const Ciphertext &rhs);
Plaintext operator*(const Scalar &lhs, const Plaintext &rhs);
Scalar operator*(const Scalar &lhs, const Scalar &rhs);

// multiplication assignement
Ciphertext &operator*=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator*=(Ciphertext &lhs, const Plaintext &rhs);
Ciphertext &operator*=(Ciphertext &lhs, const Scalar &rhs);
Plaintext &operator*=(Plaintext &lhs, const Plaintext &rhs);
Plaintext &operator*=(Plaintext &lhs, const Scalar &rhs);
Scalar &operator*=(Scalar &lhs, const Scalar &rhs);

// negation
Ciphertext operator-(const Ciphertext &lhs);
Plaintext operator-(const Plaintext &lhs);
Scalar operator-(const Scalar &lhs);

// rotation
Ciphertext operator<<(const Ciphertext &lhs, int steps);
Ciphertext operator>>(const Ciphertext &lhs, int steps);
Plaintext operator<<(const Plaintext &lhs, int steps);
Plaintext operator>>(const Plaintext &lhs, int steps);

// rotation assignement
Ciphertext &operator<<=(Ciphertext &lhs, int steps);
Ciphertext &operator>>=(Ciphertext &lhs, int steps);
Plaintext &operator<<=(Plaintext &lhs, int steps);
Plaintext &operator>>=(Plaintext &lhs, int steps);

// square, expo, multiply many, add_many, reduce*

Ciphertext square(const Ciphertext &encrypted);

Ciphertext reduce_add(const Ciphertext &encrypted);

Ciphertext add_many(const std::vector<Ciphertext> &encrypteds);

} // namespace fhecompiler
