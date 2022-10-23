#pragma once

#include "../api.hpp"
#include "../modulus.hpp"
#include "seal/seal.h"
#include <cstddef>

namespace ufhe
{
namespace seal_backend
{
  class Modulus : public ufhe::Modulus
  {
    friend class ufhe::Modulus;

  public:
    static inline std::unique_ptr<ufhe::Modulus> create(std::uint64_t value)
    {
      return ufhe::Modulus::create(ufhe::Backend::seal, value);
    }

    inline operator const seal::Modulus &() const { return modulus_; }

    inline int bit_count() const override { return modulus_.bit_count(); }

    inline std::uint64_t value() const override { return modulus_.value(); }

    inline bool is_prime() const override { return modulus_.is_prime(); }

    inline bool operator==(const ufhe::Modulus &compare) const override
    {
      return modulus_ == dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator!=(const ufhe::Modulus &compare) const override
    {
      return modulus_ != dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<(const ufhe::Modulus &compare) const override
    {
      return modulus_ < dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<=(const ufhe::Modulus &compare) const override
    {
      return modulus_ <= dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>(const ufhe::Modulus &compare) const override
    {
      return modulus_ > dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>=(const ufhe::Modulus &compare) const override
    {
      return modulus_ >= dynamic_cast<const Modulus &>(compare);
    }

    inline std::uint64_t reduce(std::uint64_t value) const override { return modulus_.reduce(value); }

  protected:
    Modulus() = default;

    inline void init(std::uint64_t value) override { modulus_ = seal::Modulus(value); }

  private:
    seal::Modulus modulus_;
  };
} // namespace seal_backend
} // namespace ufhe
