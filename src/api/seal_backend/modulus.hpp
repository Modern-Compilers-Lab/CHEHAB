#pragma once

#include "../backend.hpp"
#include "../modulus.hpp"
#include "seal/seal.h"
#include <cstddef>

namespace api
{
namespace seal_backend
{
  class Modulus : public api::Modulus
  {
    friend class api::Modulus;

  public:
    static inline std::unique_ptr<api::Modulus> create(std::uint64_t value)
    {
      return api::Modulus::create(api::Backend::seal, value);
    }

    inline operator const seal::Modulus &() const { return modulus_; }

    inline int bit_count() const override { return modulus_.bit_count(); }

    inline std::uint64_t value() const override { return modulus_.value(); }

    inline bool is_prime() const override { return modulus_.is_prime(); }

    inline bool operator==(const api::Modulus &compare) const override
    {
      return modulus_ == dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator!=(const api::Modulus &compare) const override
    {
      return modulus_ != dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<(const api::Modulus &compare) const override
    {
      return modulus_ < dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator<=(const api::Modulus &compare) const override
    {
      return modulus_ <= dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>(const api::Modulus &compare) const override
    {
      return modulus_ > dynamic_cast<const Modulus &>(compare);
    }

    inline bool operator>=(const api::Modulus &compare) const override
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
} // namespace api
