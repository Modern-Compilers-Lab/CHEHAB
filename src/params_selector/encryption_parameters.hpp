#pragma once

#include "fhecompiler_const.hpp"
#include <vector>

namespace params_selector
{

struct EncryptionParameters
{

  std::size_t poly_modulus_degree = 0; // degree of n-th cyclotomique polynomial Q where the ring R = Z[X]/Q^n[x]
  std::vector<int> coef_modulus; // q = product of primes
  std::size_t plaintext_modulus = 0; // t
  int plaintext_modulus_bit_length = 0; // bit length of
  fhecompiler::sec_level_type security_level = fhecompiler::sec_level_type::tc128; // 128-bit default security level

  EncryptionParameters() = default;

  EncryptionParameters(std::size_t n, std::vector<int> q, std::size_t p)
    : poly_modulus_degree(n), coef_modulus(q), plaintext_modulus(p)
  {}
  ~EncryptionParameters() {}

  void set_polynomial_modulus_degree(std::size_t n) { poly_modulus_degree = n; }
  void set_coef_modulus(const std::vector<int> &q) { coef_modulus = q; }
  void set_plaintext_modulus(std::size_t t)
  {
    if (plaintext_modulus_bit_length > 0)
      plaintext_modulus_bit_length = 0;
    plaintext_modulus = t;
  }
  void set_security_level(fhecompiler::sec_level_type level) { security_level = level; }
  void set_plaintext_modulus_bit_length(int bit_length)
  {
    if (plaintext_modulus > 0)
      plaintext_modulus = 0;
    plaintext_modulus_bit_length = bit_length;
  }
};

} // namespace params_selector
