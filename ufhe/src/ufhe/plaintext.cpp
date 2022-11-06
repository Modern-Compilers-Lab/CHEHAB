#include "ufhe/plaintext.hpp"

namespace ufhe
{
Plaintext::Plaintext(api::backend_type backend)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(api::backend_type backend, std::size_t coeff_count)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>(coeff_count);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(api::backend_type backend, const std::string &hex_poly)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>(hex_poly);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(const Plaintext &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Plaintext>(static_cast<const seal_backend::Plaintext &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext &Plaintext::operator=(const Plaintext &assign)
{

  switch (assign.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Plaintext>(static_cast<const seal_backend::Plaintext &>(assign.underlying()));
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
