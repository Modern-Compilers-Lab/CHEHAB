#pragma once

#include "ufhe/api/imodulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include <stdexcept>

namespace ufhe
{
class Modulus : public api::IModulus
{
  friend class CoeffModulus;
  friend class EncryptionParameters;

public:
  Modulus(std::uint64_t value = 0)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Modulus(value);
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Modulus(const Modulus &copy)
  {
    switch (copy.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(copy.underlying()));
      break;

    default:
      throw std::logic_error("instance with unknown backend");
      break;
    }
  }

  Modulus &operator=(const Modulus &assign)
  {
    delete underlying_;
    switch (assign.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(assign.underlying()));
      break;

    default:
      throw std::logic_error("instance with unknown backend");
      break;
    }
    return *this;
  }

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

  inline std::uint64_t reduce(std::uint64_t value) const override { return underlying().reduce(value); }

private:
  Modulus(const api::IModulus &imodulus)
  {
    switch (imodulus.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(imodulus));
      break;

    default:
      throw std::logic_error("instance with unknown backend");
      break;
    }
  }

  inline api::IModulus &underlying() const { return *underlying_; }

  api::IModulus *underlying_;
};
} // namespace ufhe
