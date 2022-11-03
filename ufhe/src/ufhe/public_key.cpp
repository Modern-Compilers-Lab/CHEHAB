#include "ufhe/public_key.hpp"

namespace ufhe
{
PublicKey::PublicKey()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::PublicKey();
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
