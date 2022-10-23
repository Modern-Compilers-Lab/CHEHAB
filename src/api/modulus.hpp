#pragma once

#include "api.hpp"
#include <cstdint>
#include <memory>

namespace ufhe
{
class Modulus
{
public:
  static std::unique_ptr<Modulus> create(Backend backend, std::uint64_t value);

  static inline std::unique_ptr<Modulus> create(std::uint64_t value) { return create(Backend::none, value); }

  virtual ~Modulus() = default;

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

private:
  virtual void init(std::uint64_t value) = 0;
};
} // namespace ufhe
