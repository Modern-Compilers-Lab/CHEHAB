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
  public:
    inline Modulus(std::uint64_t value) : modulus_(seal::Modulus(value)) {}

    inline operator const seal::Modulus &() const { return modulus_; }

    inline Backend backend() override { return Backend::seal; };

    inline IModulus &operator=(std::uint64_t value) override
    {
      modulus_ = value;
      return *this;
    }

    inline int bit_count() const override { return modulus_.bit_count(); }

    inline std::uint64_t value() const override { return modulus_.value(); }

    inline bool is_prime() const override { return modulus_.is_prime(); }

    inline bool operator==(const IModulus &compare) const override
    {
      return modulus_ == dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator!=(const IModulus &compare) const override
    {
      return modulus_ != dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<(const IModulus &compare) const override
    {
      return modulus_ < dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<=(const IModulus &compare) const override
    {
      return modulus_ <= dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>(const IModulus &compare) const override
    {
      return modulus_ > dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>=(const IModulus &compare) const override
    {
      return modulus_ >= dynamic_cast<const Modulus &>(compare);
    }

    inline std::uint64_t reduce(std::uint64_t value) const override { return modulus_.reduce(value); }

  private:
    seal::Modulus modulus_;
  };
} // namespace seal_backend
} // namespace ufhe
