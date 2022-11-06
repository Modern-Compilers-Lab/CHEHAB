#include "ufhe/modulus.hpp"

namespace ufhe
{
Modulus::Modulus(api::backend_type backend, std::uint64_t value)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Modulus>(value);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Modulus::Modulus(const api::Modulus &modulus)
  : underlying_(std::shared_ptr<api::Modulus>(&const_cast<api::Modulus &>(modulus), [](api::Modulus *) {}))
{}
} // namespace ufhe
