#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <cstddef>
#include <string>

namespace ufhe
{
namespace api
{
  class Plaintext : public AbstractType
  {
  public:
    virtual void resize(std::size_t coeff_count) = 0;

    virtual void set_zero(std::size_t start_coeff, std::size_t length) = 0;

    virtual std::size_t capacity() const = 0;

    virtual std::size_t coeff_count() const = 0;

    virtual std::string to_string() const = 0;

    virtual bool operator==(const Plaintext &compare) const = 0;

    virtual bool operator!=(const Plaintext &compare) const = 0;

    // TODO: Allow access to specific coefficient of the plaintext polynomial

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
