#pragma once

#include "imodulus.hpp"
#include "seal_backend/modulus.hpp"
#include <cstddef>

namespace ufhe
{
class Modulus : public IModulus
{
  friend class EncryptionParameters;

public:
  inline Modulus(Backend backend, std::uint64_t value)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::Modulus(value);
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline Modulus(std::uint64_t value) : Modulus(Backend::none, value) {}

  Modulus(const Modulus &copy) = delete;

  Modulus &operator=(const Modulus &assign) = delete;

  ~Modulus() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

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

  inline std::uint64_t reduce(std::uint64_t value) const override { return underlying_->reduce(value); }

private:
  inline IModulus &underlying() const { return *underlying_; }

  IModulus *underlying_;
};
} // namespace ufhe
