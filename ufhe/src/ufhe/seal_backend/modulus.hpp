#pragma once

#include "seal/seal.h"
#include "ufhe/api/imodulus.hpp"
#include <cstddef>

namespace ufhe
{
class Modulus;

namespace seal_backend
{
  class Modulus : public api::IModulus
  {
    friend class CoeffModulus;
    friend class EncryptionParameters;

  public:
    inline Modulus(std::uint64_t value = 0) : underlying_(seal::Modulus(value)) {}

    static inline Modulus PlainModulus(std::size_t poly_modulus_degree, int bit_size)
    {
      return Modulus(seal::PlainModulus::Batching(poly_modulus_degree, bit_size));
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline Modulus &operator=(std::uint64_t value) override
    {
      underlying_ = value;
      return *this;
    }

    inline int bit_count() const override { return underlying_.bit_count(); }

    inline std::uint64_t value() const override { return underlying_.value(); }

    inline bool is_prime() const override { return underlying_.is_prime(); }

    inline bool operator==(const api::IModulus &compare) const override
    {
      return underlying_ == dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline bool operator!=(const api::IModulus &compare) const override
    {
      return underlying_ != dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline bool operator<(const api::IModulus &compare) const override
    {
      return underlying_ < dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline bool operator<=(const api::IModulus &compare) const override
    {
      return underlying_ <= dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline bool operator>(const api::IModulus &compare) const override
    {
      return underlying_ > dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline bool operator>=(const api::IModulus &compare) const override
    {
      return underlying_ >= dynamic_cast<const Modulus &>(compare).underlying_;
    }

    inline std::uint64_t reduce(std::uint64_t value) const override { return underlying_.reduce(value); }

  private:
    inline Modulus(seal::Modulus seal_modulus) : underlying_(seal_modulus) {}

    seal::Modulus underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
