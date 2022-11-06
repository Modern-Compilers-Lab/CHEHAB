#include "ufhe/relin_keys.hpp"

namespace ufhe
{
RelinKeys::RelinKeys()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::RelinKeys>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

RelinKeys::RelinKeys(const RelinKeys &copy)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::RelinKeys>(dynamic_cast<const seal_backend::RelinKeys &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

RelinKeys &RelinKeys::operator=(const RelinKeys &assign)
{

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::RelinKeys>(dynamic_cast<const seal_backend::RelinKeys &>(assign.underlying()));
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
