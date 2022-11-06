#include "ufhe/galois_keys.hpp"

namespace ufhe
{
GaloisKeys::GaloisKeys()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::GaloisKeys>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

GaloisKeys::GaloisKeys(const GaloisKeys &copy)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::GaloisKeys>(dynamic_cast<const seal_backend::GaloisKeys &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

GaloisKeys &GaloisKeys::operator=(const GaloisKeys &assign)
{

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::GaloisKeys>(dynamic_cast<const seal_backend::GaloisKeys &>(assign.underlying()));
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
