#pragma once

#include "fheco/dsl/common.hpp"
#include <cstddef>
#include <ostream>
#include <vector>

#define MOD_BIT_COUNT_MAX 60

namespace fheco::param_select
{
class EncParams
{
public:
  EncParams(std::size_t poly_mod_degree, int plain_mod_bit_size, int coeff_mod_data_level_bit_count);

  EncParams(std::size_t poly_mod_degree, integer plain_mod, std::vector<int> coeff_mod_bit_sizes);

  int increase_coeff_mod_bit_sizes(int max_total_amount);

  int last_coeff_mod_big_prime_idx() const;

  std::vector<int> coeff_mod_data_level_bit_sizes() const;

  void print_params(std::ostream &os) const;

  inline int coeff_mod_data_level_bit_count() const { return coeff_mod_bit_count_ - coeff_mod_bit_sizes_.back(); }

  inline int poly_mod_degree() const { return poly_mod_degree_; }

  inline int plain_mod_bit_size() const { return plain_mod_bit_size_; }

  inline const std::vector<int> &coeff_mod_bit_sizes() const { return coeff_mod_bit_sizes_; }

  inline int coeff_mod_bit_count() const { return coeff_mod_bit_count_; }

private:
  std::size_t poly_mod_degree_;

  int plain_mod_bit_size_;

  std::vector<int> coeff_mod_bit_sizes_;

  int coeff_mod_bit_count_;
};
} // namespace fheco::param_select
