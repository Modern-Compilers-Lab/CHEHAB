#pragma once

#include "seal/seal.h"
#include "ufhe/api/modulus.hpp"
#include <cstddef>
#include <memory>

namespace ufhe
{
class Modulus;

namespace seal_backend
{
  class Modulus : public api::Modulus
  {
    friend class CoeffModulus;
    friend class EncryptionParams;

  public:
    Modulus(std::uint64_t value) : underlying_(std::make_shared<seal::Modulus>(value)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

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

    inline const seal::Modulus &underlying() const { return *underlying_; }

  private:
    explicit Modulus(const seal::Modulus &modulus)
      : underlying_(std::shared_ptr<seal::Modulus>(&const_cast<seal::Modulus &>(modulus), [](seal::Modulus *) {}))
    {}

    std::shared_ptr<seal::Modulus> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
