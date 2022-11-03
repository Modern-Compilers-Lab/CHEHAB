#pragma once

#include "ufhe/api/modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include <stdexcept>

namespace ufhe
{
class Modulus : public api::Modulus
{
  friend class CoeffModulus;
  friend class EncryptionParams;

public:
  Modulus(std::uint64_t value = 0);

  Modulus(const Modulus &copy);

  Modulus &operator=(const Modulus &assign);

  ~Modulus() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline Modulus &operator=(std::uint64_t value) override
  {
    underlying() = value;
    return *this;
  }

  inline int bit_count() const override { return underlying().bit_count(); }

  inline std::uint64_t value() const override { return underlying().value(); }

  inline bool is_prime() const override { return underlying().is_prime(); }

  inline bool operator==(const api::Modulus &compare) const override
  {
    return underlying() == dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline bool operator!=(const api::Modulus &compare) const override
  {
    return underlying() != dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline bool operator<(const api::Modulus &compare) const override
  {
    return underlying() < dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline bool operator<=(const api::Modulus &compare) const override
  {
    return underlying() <= dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline bool operator>(const api::Modulus &compare) const override
  {
    return underlying() > dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline bool operator>=(const api::Modulus &compare) const override
  {
    return underlying() >= dynamic_cast<const Modulus &>(compare).underlying();
  }

  inline std::uint64_t reduce(std::uint64_t value) const override { return underlying().reduce(value); }

private:
  Modulus(const api::Modulus &imodulus);

  inline api::Modulus &underlying() const { return *underlying_; }

  api::Modulus *underlying_;
};
} // namespace ufhe
