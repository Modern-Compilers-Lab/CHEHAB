#include "ufhe/secret_key.hpp"

namespace ufhe
{
SecretKey::SecretKey()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::SecretKey>();
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
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::SecretKey>(dynamic_cast<const seal_backend::SecretKey &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

SecretKey &SecretKey::operator=(const SecretKey &assign)
{

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::SecretKey>(dynamic_cast<const seal_backend::SecretKey &>(assign.underlying()));
    return *this;
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

SecretKey::SecretKey(const api::SecretKey &secret_key)
  : underlying_(std::shared_ptr<api::SecretKey>(&const_cast<api::SecretKey &>(secret_key), [](api::SecretKey *) {}))
{}
} // namespace ufhe
