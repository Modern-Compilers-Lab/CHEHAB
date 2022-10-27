#pragma once

#include <vector>

namespace params_selector
{

struct EncryptionParameters
{
  std::size_t poly_modulus_degree; // degree of n-th cyclotomique polynomial Q where the ring R = Z[X]/Q^n(x)
  std::vector<std::size_t> coef_modulus; // q = product of primes
  std::size_t plaintext_modulus; // p

  EncryptionParameters() = default;

  EncryptionParameters(std::size_t n, std::vector<std::size_t> q, std::size_t p)
    : poly_modulus_degree(n), coef_modulus(q), plaintext_modulus(p)
  {}
  ~EncryptionParameters() {}
};

} // namespace params_selector
