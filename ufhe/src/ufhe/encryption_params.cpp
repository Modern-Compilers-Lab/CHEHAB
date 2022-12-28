#include "ufhe/encryption_params.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"

namespace ufhe
{
EncryptionParams::EncryptionParams(const Scheme &scheme) : scheme_{scheme}
{
  switch (scheme.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::EncryptionParams>(static_cast<const seal_backend::Scheme &>(scheme.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  coeff_modulus_ = CoeffModulus(underlying().coeff_modulus());
  plain_modulus_ = Modulus(underlying().plain_modulus());
}

EncryptionParams::EncryptionParams(const EncryptionParams &copy) : underlying_{}, scheme_{copy.scheme_}
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionParams>(
      static_cast<const seal_backend::EncryptionParams &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  coeff_modulus_ = CoeffModulus(underlying().coeff_modulus());
  plain_modulus_ = Modulus(underlying().plain_modulus());
}

EncryptionParams &EncryptionParams::operator=(const EncryptionParams &assign)
{
  switch (assign.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionParams>(
      static_cast<const seal_backend::EncryptionParams &>(assign.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  scheme_ = Scheme(assign.scheme_);
  coeff_modulus_ = CoeffModulus(underlying().coeff_modulus());
  plain_modulus_ = Modulus(underlying().plain_modulus());
  return *this;
}

void EncryptionParams::set_poly_modulus_degree(std::size_t poly_modulus_degree)
{
  underlying_->set_poly_modulus_degree(poly_modulus_degree);
}

void EncryptionParams::set_coeff_modulus(const api::CoeffModulus &coeff_modulus)
{
  underlying_->set_coeff_modulus(safe_static_cast<const CoeffModulus &>(coeff_modulus).underlying());
}

void EncryptionParams::set_plain_modulus(const api::Modulus &plain_modulus)
{
  underlying_->set_plain_modulus(safe_static_cast<const Modulus &>(plain_modulus).underlying());
}

const Scheme &EncryptionParams::scheme() const
{
  return scheme_;
}

std::size_t EncryptionParams::poly_modulus_degree() const
{
  return underlying().poly_modulus_degree();
}

const CoeffModulus &EncryptionParams::coeff_modulus() const
{
  return coeff_modulus_;
}

const Modulus &EncryptionParams::plain_modulus() const
{
  return plain_modulus_;
}
} // namespace ufhe
