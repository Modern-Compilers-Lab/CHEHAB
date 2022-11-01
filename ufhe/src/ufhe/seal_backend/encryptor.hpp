#pragma once

#include "seal/seal.h"
#include "ufhe/api/iencryptor.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryptioncontext.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/publickey.hpp"
#include "ufhe/seal_backend/secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Encryptor : public api::IEncryptor
  {
  public:
    inline Encryptor(const EncryptionContext &context, const PublicKey &public_key)
      : underlying_(seal::Encryptor(context.underlying_, public_key.underlying_))
    {}

    inline Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::Encryptor(context.underlying_, secret_key.underlying_))
    {}

    inline Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
      : underlying_(seal::Encryptor(context.underlying_, public_key.underlying_, secret_key.underlying_))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void encrypt(const api::IPlaintext &plain, api::ICiphertext &destination) const override
    {
      underlying_.encrypt(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void encrypt_symmetric(const api::IPlaintext &plain, api::ICiphertext &destination) const override
    {
      underlying_.encrypt_symmetric(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

  private:
    seal::Encryptor underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
