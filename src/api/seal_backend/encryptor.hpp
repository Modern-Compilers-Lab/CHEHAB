#pragma once

#include "../iencryptor.hpp"
#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "plaintext.hpp"
#include "publickey.hpp"
#include "seal/seal.h"
#include "secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Encryptor : public Implementation, public IEncryptor
  {
  public:
    inline Encryptor(const EncryptionContext &context, const PublicKey &public_key)
      : Encryptor(new seal::Encryptor(context.underlying(), public_key.underlying()), true)
    {}

    inline Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : Encryptor(new seal::Encryptor(context.underlying(), secret_key.underlying()), true)
    {}

    inline Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
      : Encryptor(new seal::Encryptor(context.underlying(), public_key.underlying(), secret_key.underlying()), true)
    {}

    inline Encryptor(const Encryptor &copy) : Encryptor(copy.underlying_, false) {}

    Encryptor &operator=(const Encryptor &assign) = delete;

    inline ~Encryptor()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline void encrypt(const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying().encrypt(
        dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void encrypt_symmetric(const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying().encrypt_symmetric(
        dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

  private:
    inline Encryptor(seal::Encryptor *seal_encryptor, bool is_owner) : underlying_(seal_encryptor), is_owner_(is_owner)
    {}

    inline seal::Encryptor &underlying() const { return *underlying_; }

    seal::Encryptor *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
