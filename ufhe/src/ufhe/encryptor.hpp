#pragma once

#include "ufhe/api/encryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/public_key.hpp"
#include "ufhe/seal_backend/encryptor.hpp"
#include "ufhe/secret_key.hpp"
#include <memory>

namespace ufhe
{
class Encryptor : public api::Encryptor
{
public:
  Encryptor(const EncryptionContext &context, const PublicKey &public_key);

  Encryptor(const EncryptionContext &context, const SecretKey &secret_key);

  Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().encrypt(
      dynamic_cast<const Plaintext &>(plain).underlying(), *dynamic_cast<Ciphertext &>(destination).underlying_);
  }

  inline void encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().encrypt_symmetric(
      dynamic_cast<const Plaintext &>(plain).underlying(), *dynamic_cast<Ciphertext &>(destination).underlying_);
  }

  inline const api::Encryptor &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Encryptor> underlying_;
};
} // namespace ufhe
