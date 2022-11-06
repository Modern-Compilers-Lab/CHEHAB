#pragma once

#include "ufhe/api/modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include <memory>
#include <stdexcept>

namespace ufhe
{
class Modulus : public api::Modulus
{
  friend class CoeffModulus;
  friend class EncryptionParams;

public:
  Modulus(std::uint64_t value);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

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

  inline const api::Modulus &underlying() const { return *underlying_; }

private:
  Modulus() = default;

  explicit Modulus(const api::Modulus &modulus);

  std::shared_ptr<api::Modulus> underlying_;
};
} // namespace ufhe
