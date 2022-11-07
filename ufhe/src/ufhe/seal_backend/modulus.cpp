#include "ufhe/seal_backend/modulus.hpp"
#include "seal/modulus.h"

namespace ufhe
{
namespace seal_backend
{
  Modulus::Modulus(std::uint64_t value) : underlying_(std::make_shared<seal::Modulus>(value)) {}

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

  Modulus::Modulus(const seal::Modulus &modulus)
    : underlying_(std::shared_ptr<seal::Modulus>(&const_cast<seal::Modulus &>(modulus), [](seal::Modulus *) {}))
  {}
} // namespace seal_backend
} // namespace ufhe
