#pragma once

#include "modulus.hpp"
#include <cstddef>
#include <vector>

namespace api
{
class EncryptionParameters
{
public:
  virtual void set_poly_modulus_degree(const std::size_t poly_modulus_degree) = 0;

  virtual void set_coeff_modulus(const std::vector<Modulus> &coeff_modulus) = 0;

  virtual void set_plain_modulus(const Modulus &plain_modulus) = 0;
};
} // namespace api
