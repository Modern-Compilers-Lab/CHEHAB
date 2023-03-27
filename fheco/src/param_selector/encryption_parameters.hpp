#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace param_selector
{
class EncryptionParameters
{
public:
  EncryptionParameters() {}

  EncryptionParameters(
    std::size_t poly_modulus_degree, int plain_modulus_bit_size, const std::vector<int> &coeff_mod_bit_sizes,
    int coeff_mod_bit_count)
    : poly_modulus_degree_(poly_modulus_degree), plain_modulus_bit_size_(plain_modulus_bit_size),
      coeff_mod_bit_sizes_(coeff_mod_bit_sizes), coeff_mod_bit_count_(coeff_mod_bit_count)
  {}

  int poly_modulus_degree() const { return poly_modulus_degree_; }

  int plain_modulus_bit_size() const { return plain_modulus_bit_size_; }

  const std::vector<int> &coeff_mod_bit_sizes() const { return coeff_mod_bit_sizes_; }

  int coeff_mod_bit_count() const { return coeff_mod_bit_count_; }

  void print_parameters() const;

private:
  std::size_t poly_modulus_degree_ = 0;
  int plain_modulus_bit_size_ = 0;
  std::vector<int> coeff_mod_bit_sizes_;
  int coeff_mod_bit_count_ = 0;
};
} // namespace param_selector
