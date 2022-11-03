#include "ufhe/key_generator.hpp"

namespace ufhe
{
KeyGenerator::KeyGenerator(EncryptionContext &context)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      new seal_backend::KeyGenerator(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
    secret_key_ = new SecretKey(underlying().secret_key());
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

KeyGenerator::KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
{
  secret_key_ = new SecretKey(secret_key);
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::KeyGenerator(
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
