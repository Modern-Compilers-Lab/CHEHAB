#pragma once

#include "interface.hpp"
#include <cstddef>
#include <string>

namespace ufhe
{
class IPlaintext : public virtual Interface
{
public:
  virtual void resize(std::size_t coeff_count) = 0;

  virtual void set_zero(std::size_t start_coeff, std::size_t length) = 0;

  virtual std::size_t capacity() const = 0;

  virtual std::size_t coeff_count() const = 0;

  virtual std::string to_string() const = 0;

  virtual bool operator==(const IPlaintext &compare) const = 0;

  virtual bool operator!=(const IPlaintext &compare) const = 0;

  // TODO: Allow access to specific coefficient of the plaintext polynomial

  // TODO: Serialization support
};
} // namespace ufhe
