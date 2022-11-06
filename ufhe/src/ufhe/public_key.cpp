#include "ufhe/public_key.hpp"

namespace ufhe
{
PublicKey::PublicKey()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::PublicKey>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

PublicKey::PublicKey(const PublicKey &copy)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::PublicKey>(dynamic_cast<const seal_backend::PublicKey &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

PublicKey &PublicKey::operator=(const PublicKey &assign)
{

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::PublicKey>(dynamic_cast<const seal_backend::PublicKey &>(assign.underlying()));
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
} // namespace ufhe
