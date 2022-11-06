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
} // namespace ufhe
