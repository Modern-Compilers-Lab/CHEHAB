#include "ufhe/ciphertext.hpp"

namespace ufhe
{
Ciphertext::Ciphertext()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Ciphertext>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Ciphertext::Ciphertext(const EncryptionContext &context) : underlying_()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Ciphertext>(
      dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Ciphertext::Ciphertext(const Ciphertext &copy)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Ciphertext>(dynamic_cast<const seal_backend::Ciphertext &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Ciphertext &Ciphertext::operator=(const Ciphertext &assign)
{

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Ciphertext>(dynamic_cast<const seal_backend::Ciphertext &>(assign.underlying()));
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
