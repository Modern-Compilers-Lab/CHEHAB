#include "ufhe/encryption_params.hpp"

namespace ufhe
{
EncryptionParams::EncryptionParams(const Scheme &scheme)
  : scheme_(scheme), coeff_modulus_(new CoeffModulus()), plain_modulus_(new Modulus())
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::EncryptionParams(dynamic_cast<const seal_backend::Scheme &>(scheme.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}
} // namespace ufhe
