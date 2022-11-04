#include "ufhe/ciphertext.hpp"

namespace ufhe
{
Ciphertext::Ciphertext()
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Ciphertext();
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
  delete underlying_;
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Ciphertext(dynamic_cast<const seal_backend::Ciphertext &>(assign.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  return *this;
}

Ciphertext::Ciphertext(const EncryptionContext &context)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      new seal_backend::Ciphertext(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
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
