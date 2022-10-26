#pragma once

#include "api.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace ufhe
{
class IModulus
{
public:
  using vector = std::vector<std::reference_wrapper<IModulus>>;

  virtual ~IModulus() = default;

  virtual Backend backend() = 0;

  virtual IModulus &operator=(std::uint64_t value) = 0;

  virtual int bit_count() const = 0;

  virtual std::uint64_t value() const = 0;

  virtual bool is_prime() const = 0;

  virtual bool operator==(const IModulus &compare) const = 0;

  virtual bool operator!=(const IModulus &compare) const = 0;

  virtual bool operator<(const IModulus &compare) const = 0;

  virtual bool operator<=(const IModulus &compare) const = 0;

  virtual bool operator>(const IModulus &compare) const = 0;

  virtual bool operator>=(const IModulus &compare) const = 0;

  virtual std::uint64_t reduce(std::uint64_t value) const = 0;

  // TODO: Serialization support

protected:
  IModulus() = default;

  IModulus(const IModulus &) = default;

  IModulus &operator=(const IModulus &) = default;

  IModulus(IModulus &&) = default;

  IModulus &operator=(IModulus &&) = default;
};
} // namespace ufhe
