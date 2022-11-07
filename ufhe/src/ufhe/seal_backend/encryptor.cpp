#include "ufhe/seal_backend/encryptor.hpp"
#include "seal/encryptor.h"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/public_key.hpp"
#include "ufhe/seal_backend/secret_key.hpp"

namespace ufhe
{
namespace seal_backend
{
  Encryptor::Encryptor(const EncryptionContext &context, const PublicKey &public_key)
    : underlying_(std::make_shared<seal::Encryptor>(context.underlying(), public_key.underlying()))
  {}

  Encryptor::Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
    : underlying_(std::make_shared<seal::Encryptor>(context.underlying(), secret_key.underlying()))
  {}

  Encryptor::Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
    : underlying_(
        std::make_shared<seal::Encryptor>(context.underlying(), public_key.underlying(), secret_key.underlying()))
  {}

  void Encryptor::encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const
  {
    underlying().encrypt(
      safe_static_cast<const Plaintext &>(plain).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Encryptor::encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const
  {
    underlying().encrypt_symmetric(
      safe_static_cast<const Plaintext &>(plain).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }
} // namespace seal_backend
} // namespace ufhe
