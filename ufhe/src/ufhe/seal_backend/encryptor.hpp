#pragma once

#include "seal/seal.h"
#include "ufhe/api/encryptor.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/public_key.hpp"
#include "ufhe/seal_backend/secret_key.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Encryptor : public api::Encryptor
  {
  public:
    Encryptor(const EncryptionContext &context, const PublicKey &public_key)
      : underlying_(seal::Encryptor(context.underlying_, public_key.underlying_))
    {}

    Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::Encryptor(context.underlying_, secret_key.underlying_))
    {}

    Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
      : underlying_(seal::Encryptor(context.underlying_, public_key.underlying_, secret_key.underlying_))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const override
    {
      underlying_.encrypt(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const override
    {
      underlying_.encrypt_symmetric(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

  private:
    seal::Encryptor underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
