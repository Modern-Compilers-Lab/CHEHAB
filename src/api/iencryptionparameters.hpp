#pragma once

#include "icoeff_modulus.hpp"
#include "imodulus.hpp"
#include "interface.hpp"
#include <cstddef>
#include <cstdint>

namespace ufhe
{
class ISchemeType : public virtual Interface
{};

class IEncryptionParameters
{
public:
  virtual void set_poly_modulus_degree(std::size_t poly_modulus_degree) = 0;

  virtual void set_coeff_modulus(const ICoeffModulus &coeff_modulus) = 0;

  virtual void set_plain_modulus(const IModulus &plain_modulus) = 0;

  // TODO: Virtual setter for the random number generator factory

  virtual const ISchemeType &scheme() const = 0;

  virtual std::size_t poly_modulus_degree() const = 0;

  virtual const ICoeffModulus &coeff_modulus() const = 0;

  virtual const IModulus &plain_modulus() const = 0;

  // TODO: Virtual getter for the random number generator factory

  // TODO: Serialization support
};
} // namespace ufhe
