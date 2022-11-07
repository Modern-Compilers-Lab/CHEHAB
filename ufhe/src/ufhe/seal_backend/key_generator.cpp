#include "ufhe/seal_backend/key_generator.hpp"
#include "seal/keygenerator.h"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include "ufhe/seal_backend/public_key.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"

namespace ufhe
{
namespace seal_backend
{
  KeyGenerator::KeyGenerator(const EncryptionContext &context)
    : underlying_(std::make_shared<seal::KeyGenerator>(context.underlying())), secret_key_(underlying_->secret_key())
  {}

  KeyGenerator::KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
    : underlying_(std::make_shared<seal::KeyGenerator>(context.underlying(), secret_key.underlying())),
      secret_key_(underlying_->secret_key())
  {}

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
    underlying_->create_relin_keys(*safe_static_cast<RelinKeys &>(destination).underlying_);
  }

  void KeyGenerator::create_galois_keys(api::GaloisKeys &destination) const
  {
    underlying_->create_galois_keys(*safe_static_cast<GaloisKeys &>(destination).underlying_);
  }
} // namespace seal_backend
} // namespace ufhe
