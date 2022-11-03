#pragma once

#include "ufhe/api/encryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/public_key.hpp"
#include "ufhe/seal_backend/encryptor.hpp"
#include "ufhe/secret_key.hpp"

namespace ufhe
{
class Encryptor : public api::Encryptor
{
public:
  Encryptor(const EncryptionContext &context, const PublicKey &public_key);

  Encryptor(const EncryptionContext &context, const SecretKey &secret_key);

  Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key);

  Encryptor(const Encryptor &copy) = delete;

  Encryptor &operator=(const Encryptor &assign) = delete;

  ~Encryptor() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().encrypt(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().encrypt_symmetric(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

private:
  inline api::Encryptor &underlying() const { return *underlying_; }

  api::Encryptor *underlying_;
};
} // namespace ufhe
