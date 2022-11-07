#include "ufhe/modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/modulus.hpp"

namespace ufhe
{
Modulus::Modulus(api::backend_type backend, std::uint64_t value)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Modulus>(value);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Modulus::Modulus(std::uint64_t value) : Modulus(Config::backend(), value) {}

int Modulus::bit_count() const
{
  return underlying().bit_count();
}

std::uint64_t Modulus::value() const
{
  return underlying().value();
}

bool Modulus::is_prime() const
{
  return underlying().is_prime();
}

bool Modulus::operator==(const api::Modulus &compare) const
{
  return underlying() == safe_static_cast<const Modulus &>(compare).underlying();
}

bool Modulus::operator!=(const api::Modulus &compare) const
{
  return underlying() != safe_static_cast<const Modulus &>(compare).underlying();
}

bool Modulus::operator<(const api::Modulus &compare) const
{
  return underlying() < safe_static_cast<const Modulus &>(compare).underlying();
}

bool Modulus::operator<=(const api::Modulus &compare) const
{
  return underlying() <= safe_static_cast<const Modulus &>(compare).underlying();
}

bool Modulus::operator>(const api::Modulus &compare) const
{
  return underlying() > safe_static_cast<const Modulus &>(compare).underlying();
}

bool Modulus::operator>=(const api::Modulus &compare) const
{
  return underlying() >= safe_static_cast<const Modulus &>(compare).underlying();
}

std::uint64_t Modulus::reduce(std::uint64_t value) const
{
  return underlying().reduce(value);
}

Modulus::Modulus(const api::Modulus &modulus)
  : underlying_(std::shared_ptr<api::Modulus>(&const_cast<api::Modulus &>(modulus), [](api::Modulus *) {}))
{}
} // namespace ufhe
