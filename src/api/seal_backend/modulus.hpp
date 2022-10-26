#pragma once

#include "../imodulus.hpp"
#include "seal/seal.h"
#include <cstddef>

namespace ufhe
{
class Modulus;

namespace seal_backend
{
  class Modulus : public IModulus
  {
    friend class EncryptionParameters;

  public:
    inline Modulus(std::uint64_t value) : Modulus(new seal::Modulus(value), true) {}

    inline Modulus(const Modulus &copy) : Modulus(copy.underlying_, false) {}

    Modulus &operator=(const Modulus &assign) = delete;

    inline ~Modulus()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline Backend backend() override { return Backend::seal; };

    inline IModulus &operator=(std::uint64_t value) override
    {
      underlying() = value;
      return *this;
    }

    inline int bit_count() const override { return underlying().bit_count(); }

    inline std::uint64_t value() const override { return underlying().value(); }

    inline bool is_prime() const override { return underlying().is_prime(); }

    inline bool operator==(const IModulus &compare) const override
    {
      return underlying() == dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline bool operator!=(const IModulus &compare) const override
    {
      return underlying() != dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline bool operator<(const IModulus &compare) const override
    {
      return underlying() < dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline bool operator<=(const IModulus &compare) const override
    {
      return underlying() <= dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline bool operator>(const IModulus &compare) const override
    {
      return underlying() > dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline bool operator>=(const IModulus &compare) const override
    {
      return underlying() >= dynamic_cast<const Modulus &>(compare).underlying();
    }

    inline std::uint64_t reduce(std::uint64_t value) const override { return (*underlying_).reduce(value); }

  private:
    inline Modulus(seal::Modulus *seal_modulus, bool is_owner) : underlying_(seal_modulus), is_owner_(is_owner) {}

    inline seal::Modulus &underlying() const { return *underlying_; }

    seal::Modulus *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
