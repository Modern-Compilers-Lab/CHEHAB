#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <cstdint>

namespace ufhe
{
namespace api
{
  class Ciphertext : public AbstractType
  {
  public:
    // TODO: Constructor from context and a specefic params_id_type from the modulus switching chain

    // TODO: resize

    // TODO: Allow access to a particular polynomial in the ciphertext data

    virtual std::size_t coeff_modulus_size() const = 0;

    virtual std::size_t poly_modulus_degree() const = 0;

    virtual std::size_t size() const = 0;

    virtual bool is_transparent() const = 0;

    virtual double &scale() const = 0;

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
