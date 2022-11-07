#pragma once

#include "ufhe/api/encryptor.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext;
class PublicKey;
class SecretKey;

class Encryptor : public api::Encryptor
{
public:
  Encryptor(const EncryptionContext &context, const PublicKey &public_key);

  Encryptor(const EncryptionContext &context, const SecretKey &secret_key);

  Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key);

  Encryptor(const Encryptor &copy);

  Encryptor &operator=(const Encryptor &assign);

  Encryptor(Encryptor &&source) = default;

  Encryptor &operator=(Encryptor &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  void encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const override;

  void encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const override;

  inline const api::Encryptor &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Encryptor> underlying_;
};
} // namespace ufhe
