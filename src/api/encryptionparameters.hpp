#pragma once

#include "modulus.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace api
{

class SchemeType
{
public:
  SchemeType(std::uint8_t scheme_id) { init(scheme_id); }

  virtual ~SchemeType() {}

private:
  SchemeType() {}

  virtual void init(std::uint8_t scheme_id) = 0;
};

class EncryptionParameters
{
public:
  EncryptionParameters(const SchemeType &scheme) { init(scheme); }

  virtual ~EncryptionParameters() {}

  virtual void set_poly_modulus_degree(const std::size_t poly_modulus_degree) = 0;

  virtual void set_coeff_modulus(const std::vector<Modulus> &coeff_modulus) = 0;

  virtual void set_plain_modulus(const Modulus &plain_modulus) = 0;

  // TODO: Virtual setter for the random number generator factory

  virtual const SchemeType &scheme() const = 0;

  virtual std::size_t poly_modulus_degree() const = 0;

  virtual const std::vector<Modulus> &coeff_modulus() const = 0;

  virtual const Modulus &plain_modulus() const = 0;

  // TODO: Virtual getter for the random number generator factory

  // TODO: Serialization support

private:
  virtual void init(const SchemeType &scheme_type) = 0;
};
} // namespace api
