#include "ufhe/key_generator.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/public_key.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/key_generator.hpp"

namespace ufhe
{
KeyGenerator::KeyGenerator(EncryptionContext &context)
{
  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::KeyGenerator>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()));
    secret_key_ = SecretKey(underlying().secret_key());
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
  if (context.backend() != secret_key.backend())
    throw std::invalid_argument("backend ambiguity, arguments must have the same backend");

  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::KeyGenerator>(
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
  secret_key_ = SecretKey(secret_key.underlying());
}

const SecretKey &KeyGenerator::secret_key() const
{
  return secret_key_;
}

void KeyGenerator::create_public_key(api::PublicKey &destination) const
{
  underlying().create_public_key(*safe_static_cast<PublicKey &>(destination).underlying_);
}

void KeyGenerator::create_relin_keys(api::RelinKeys &destination) const
{
  underlying().create_relin_keys(*safe_static_cast<RelinKeys &>(destination).underlying_);
}

void KeyGenerator::create_galois_keys(api::GaloisKeys &destination) const
{
  underlying().create_galois_keys(*safe_static_cast<GaloisKeys &>(destination).underlying_);
}
} // namespace ufhe
