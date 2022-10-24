#pragma once

#include "api.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace ufhe
{
class Modulus
{
public:
  using ptr = std::unique_ptr<Modulus>;

  using vector = std::vector<ptr>;

  static ptr create(Backend backend, std::uint64_t value = 0);

  static inline ptr create(std::uint64_t value = 0) { return create(Backend::none, value); }

  virtual ptr clone() = 0;

  virtual ~Modulus() = default;

  virtual Modulus &operator=(std::uint64_t value) = 0;

  virtual int bit_count() const = 0;

  virtual std::uint64_t value() const = 0;

  virtual bool is_prime() const = 0;

  virtual bool operator==(const Modulus &compare) const = 0;

  virtual bool operator!=(const Modulus &compare) const = 0;

  virtual bool operator<(const Modulus &compare) const = 0;

  virtual bool operator<=(const Modulus &compare) const = 0;

  virtual bool operator>(const Modulus &compare) const = 0;

  virtual bool operator>=(const Modulus &compare) const = 0;

  virtual std::uint64_t reduce(std::uint64_t value) const = 0;

  // TODO: Serialization support

protected:
  Modulus() = default;

  Modulus(const Modulus &) = default;

  Modulus &operator=(const Modulus &) = default;

  Modulus(Modulus &&) = default;

  Modulus &operator=(Modulus &&) = default;
};
} // namespace ufhe
