#include "ufhe/modulus.hpp"

namespace ufhe
{
Modulus::Modulus(std::uint64_t value)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Modulus(value);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Modulus::Modulus(const Modulus &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(copy.underlying()));
    break;

  default:
    throw std::logic_error("instance with unknown backend");
    break;
  }
}

Modulus &Modulus::operator=(const Modulus &assign)
{
  delete underlying_;
  switch (assign.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(assign.underlying()));
    break;

  default:
    throw std::logic_error("instance with unknown backend");
    break;
  }
  return *this;
}

Modulus::Modulus(const api::Modulus &imodulus)
{
  switch (imodulus.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Modulus(dynamic_cast<const seal_backend::Modulus &>(imodulus));
    break;

  default:
    throw std::logic_error("instance with unknown backend");
    break;
  }
}
} // namespace ufhe
