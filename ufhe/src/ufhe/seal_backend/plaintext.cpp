#include "ufhe/seal_backend/plaintext.hpp"
#include "seal/plaintext.h"

namespace ufhe
{
namespace seal_backend
{
  Plaintext::Plaintext() : underlying_(std::make_shared<seal::Plaintext>()) {}

  Plaintext::Plaintext(std::size_t coeff_count) : underlying_(std::make_shared<seal::Plaintext>(coeff_count)) {}

  Plaintext::Plaintext(const std::string &hex_poly) : underlying_(std::make_shared<seal::Plaintext>(hex_poly)) {}

  Plaintext::Plaintext(const Plaintext &copy) : underlying_(std::make_shared<seal::Plaintext>(copy.underlying())) {}

  Plaintext &Plaintext::operator=(const Plaintext &assign)
  {
    underlying_ = std::make_shared<seal::Plaintext>(assign.underlying());
    return *this;
  }

  void Plaintext::resize(std::size_t coeff_count)
  {
    underlying_->resize(coeff_count);
  }

  void Plaintext::set_zero(std::size_t start_coeff, std::size_t length)
  {
    underlying_->set_zero(start_coeff, length);
  }

  std::size_t Plaintext::capacity() const
  {
    return underlying().capacity();
  }

  std::size_t Plaintext::coeff_count() const
  {
    return underlying().coeff_count();
  }

  std::string Plaintext::to_string() const
  {
    return underlying().to_string();
  }

  bool Plaintext::operator==(const api::Plaintext &compare) const
  {
    return underlying() == safe_static_cast<const Plaintext &>(compare).underlying();
  }

  bool Plaintext::operator!=(const api::Plaintext &compare) const
  {
    return underlying() != safe_static_cast<const Plaintext &>(compare).underlying();
  }
} // namespace seal_backend
} // namespace ufhe
