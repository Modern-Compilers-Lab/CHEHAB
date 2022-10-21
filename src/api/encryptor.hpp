#pragma once

#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "plaintext.hpp"
#include "publickey.hpp"
#include "secretkey.hpp"

namespace api
{
class Encryptor
{
public:
  Encryptor(const EncryptionContext &context, const PublicKey &public_key) { init(context, public_key); }

  Encryptor(const EncryptionContext &context, const SecretKey &secret_key) { init(context, secret_key); }

  Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
  {
    init(context, public_key, secret_key);
  }

  virtual void encrypt(const Plaintext &plain, Ciphertext &destination) const = 0;

  virtual void encrypt_symmetric(const Plaintext &plain, Ciphertext &destination) const = 0;

  // TODO: allow creating seeded objects

private:
  Encryptor() {}

  virtual void init(const EncryptionContext &context, const PublicKey &public_key) = 0;

  virtual void init(const EncryptionContext &context, const SecretKey &secret_key) = 0;

  virtual void init(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key) = 0;
};
} // namespace api
