#include "ufhe/seal_backend/decryptor.hpp"
#include "seal/decryptor.h"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/secret_key.hpp"

namespace ufhe
{
namespace seal_backend
{
  Decryptor::Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
    : underlying_(std::make_shared<seal::Decryptor>(context.underlying(), secret_key.underlying()))
  {}

  void Decryptor::decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const
  {
    underlying_->decrypt(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      *safe_static_cast<Plaintext &>(destination).underlying_);
  }

  int Decryptor::invariant_noise_budget(const api::Ciphertext &encrypted) const
  {
    return underlying_->invariant_noise_budget(safe_static_cast<const Ciphertext &>(encrypted).underlying());
  }
} // namespace seal_backend
} // namespace ufhe
