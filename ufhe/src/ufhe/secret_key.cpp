#include "ufhe/secret_key.hpp"

namespace ufhe
{
SecretKey::SecretKey()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::SecretKey();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

SecretKey::SecretKey(const SecretKey &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::SecretKey(dynamic_cast<const seal_backend::SecretKey &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

SecretKey::SecretKey(const api::SecretKey &isecret_key)
{
  switch (isecret_key.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::SecretKey(dynamic_cast<const seal_backend::SecretKey &>(isecret_key));
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
