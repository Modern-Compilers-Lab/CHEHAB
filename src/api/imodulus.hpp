#pragma once

#include "interface.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace ufhe
{
class IModulus : public Interface
{
public:
  using vector = std::vector<std::reference_wrapper<const IModulus>>;

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
};
} // namespace ufhe
