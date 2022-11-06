#include "ufhe/decryptor.hpp"

namespace ufhe
{
Decryptor::Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Decryptor>(
      dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()),
      dynamic_cast<const seal_backend::SecretKey &>(secret_key.underlying()));
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
