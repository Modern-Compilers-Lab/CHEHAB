#pragma once

#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/plaintext.hpp"
#include <cstdint> 

namespace fheco
{
// addition
Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator+(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator+(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs);

// addition assignement
Ciphertext &operator+=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator+=(Ciphertext &lhs, const Plaintext &rhs);
Plaintext &operator+=(Plaintext &lhs, const Plaintext &rhs);

// subtraction
Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator-(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator-(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs);

// subtraction assignement
Ciphertext &operator-=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator-=(Ciphertext &lhs, const Plaintext &rhs);
Plaintext &operator-=(Plaintext &lhs, const Plaintext &rhs);

// multiplication
Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator*(const Ciphertext &lhs, const Plaintext &rhs);
Ciphertext operator*(const Plaintext &lhs, const Ciphertext &rhs);
Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs);

// multiplication assignement
Ciphertext &operator*=(Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext &operator*=(Ciphertext &lhs, const Plaintext &rhs);
Plaintext &operator*=(Plaintext &lhs, const Plaintext &rhs);

// negation
Ciphertext operator-(const Ciphertext &arg);
Plaintext operator-(const Plaintext &arg);

// rotation
Ciphertext operator<<(const Ciphertext &arg, int steps);
Ciphertext operator>>(const Ciphertext &arg, int steps);
Plaintext operator<<(const Plaintext &arg, int steps);
Plaintext operator>>(const Plaintext &arg, int steps);

// rotation assignement
Ciphertext &operator<<=(Ciphertext &arg, int steps);
Ciphertext &operator>>=(Ciphertext &arg, int steps);
Plaintext &operator<<=(Plaintext &arg, int steps);
Plaintext &operator>>=(Plaintext &arg, int steps);

// encryption
Ciphertext encrypt(const Plaintext &arg);

// subscript read
Ciphertext emulate_subscripted_read(const Ciphertext &arg);
Plaintext emulate_subscripted_read(const Plaintext &arg);

// subscript write
void emulate_subscripted_write(Ciphertext &lhs, const Ciphertext &rhs);
void emulate_subscripted_write(Plaintext &lhs, const Plaintext &rhs);

// square
Ciphertext square(const Ciphertext &arg);
Plaintext square(const Plaintext &arg);

// add_many
Ciphertext add_many(const std::vector<Ciphertext> &args);
Plaintext add_many(const std::vector<Plaintext> &args);

// mul_many
Ciphertext mul_many(const std::vector<Ciphertext> &args);
Plaintext mul_many(const std::vector<Plaintext> &args);

// exponentiate
Ciphertext exponentiate(const Ciphertext &arg, std::uint64_t exponent);
Plaintext exponentiate(const Plaintext &arg, std::uint64_t exponent);

// reduce_add
Ciphertext reduce_add(const Ciphertext &arg);
Plaintext reduce_add(const Plaintext &arg);

// reduce_mul
Ciphertext reduce_mul(const Ciphertext &arg);
Plaintext reduce_mul(const Plaintext &arg);
/*************************************************/
Ciphertext SumVec(const Ciphertext &arg, int size);
} // namespace fheco
