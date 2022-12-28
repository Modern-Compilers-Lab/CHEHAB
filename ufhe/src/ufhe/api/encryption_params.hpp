#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <cstddef>

namespace ufhe
{
namespace api
{
  class CoeffModulus;
  class Modulus;
  class Scheme;

  class EncryptionParams : public AbstractType
  {
  public:
    virtual void set_poly_modulus_degree(std::size_t poly_modulus_degree) = 0;

    virtual void set_coeff_modulus(const CoeffModulus &coeff_modulus) = 0;

    virtual void set_plain_modulus(const Modulus &plain_modulus) = 0;

    // TODO: Virtual setter for the random number generator factory

    virtual const Scheme &scheme() const = 0;

    virtual std::size_t poly_modulus_degree() const = 0;

    virtual const CoeffModulus &coeff_modulus() const = 0;

    virtual const Modulus &plain_modulus() const = 0;

    // TODO: Virtual getter for the random number generator factory

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
