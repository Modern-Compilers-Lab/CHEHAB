#pragma once

#include "api.hpp"
#include "imodulus.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ufhe
{
class ISchemeType
{
public:
  virtual ~ISchemeType() = default;

  virtual Backend backend() = 0;

protected:
  ISchemeType() = default;

  ISchemeType(const ISchemeType &copy) = default;

  ISchemeType &operator=(const ISchemeType &assign) = default;

  ISchemeType(ISchemeType &&source) = default;

  ISchemeType &operator=(ISchemeType &&assign) = default;
};

class IEncryptionParameters
{
public:
  virtual ~IEncryptionParameters() = default;

  virtual Backend backend() = 0;

  virtual void set_poly_modulus_degree(std::size_t poly_modulus_degree) = 0;

  virtual void set_coeff_modulus(const IModulus::vector &coeff_modulus) = 0;

  virtual void set_plain_modulus(const IModulus &plain_modulus) = 0;

  // TODO: Virtual setter for the random number generator factory

  virtual const ISchemeType &scheme() const = 0;

  virtual std::size_t poly_modulus_degree() const = 0;

  // TODO: Try to make IModulus::vector coeff_modulus() const again
  virtual IModulus::vector coeff_modulus() = 0;

  virtual const IModulus &plain_modulus() const = 0;

  // TODO: Virtual getter for the random number generator factory

  // TODO: Serialization support

protected:
  IEncryptionParameters() = default;

  IEncryptionParameters(const IEncryptionParameters &) = default;

  IEncryptionParameters &operator=(const IEncryptionParameters &) = default;

  IEncryptionParameters(IEncryptionParameters &&) = default;

  IEncryptionParameters &operator=(IEncryptionParameters &&) = default;
};
} // namespace ufhe
