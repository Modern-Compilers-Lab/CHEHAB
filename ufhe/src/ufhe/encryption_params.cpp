#include "ufhe/encryption_params.hpp"

namespace ufhe
{
EncryptionParams::EncryptionParams(const Scheme &scheme) : scheme_(scheme)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::EncryptionParams>(dynamic_cast<const seal_backend::Scheme &>(scheme.underlying()));
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

EncryptionParams::EncryptionParams(const EncryptionParams &copy) : underlying_(), scheme_(copy.scheme_)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionParams>(
      dynamic_cast<const seal_backend::EncryptionParams &>(copy.underlying()));
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
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionParams>(
      dynamic_cast<const seal_backend::EncryptionParams &>(assign.underlying()));
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
} // namespace ufhe
