#pragma once

#include "../iencryptor.hpp"
#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "plaintext.hpp"
#include "publickey.hpp"
#include "seal/seal.h"
#include "secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Encryptor : public IEncryptor
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

    inline Backend backend() const override { return Backend::seal; }

    inline void encrypt(const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying_.encrypt(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void encrypt_symmetric(const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying_.encrypt_symmetric(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

  private:
    seal::Encryptor underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
