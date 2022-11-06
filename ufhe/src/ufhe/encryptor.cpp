#include "ufhe/encryptor.hpp"

namespace ufhe
{
Encryptor::Encryptor(const EncryptionContext &context, const PublicKey &public_key)
{
  if (context.backend() != public_key.backend())
    throw std::invalid_argument("backend ambiguity, arguments must have the same backend");

  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Encryptor>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()),
      static_cast<const seal_backend::PublicKey &>(public_key.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Encryptor::Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
{
  if (context.backend() != secret_key.backend())
    throw std::invalid_argument("backend ambiguity, arguments must have the same backend");

  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Encryptor>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()),
      static_cast<const seal_backend::SecretKey &>(secret_key.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Encryptor::Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
{
  if (context.backend() != public_key.backend() || context.backend() != secret_key.backend())
    throw std::invalid_argument("backend ambiguity, arguments must have the same backend");

  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Encryptor>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()),
      static_cast<const seal_backend::PublicKey &>(public_key.underlying()),
      static_cast<const seal_backend::SecretKey &>(secret_key.underlying()));
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
