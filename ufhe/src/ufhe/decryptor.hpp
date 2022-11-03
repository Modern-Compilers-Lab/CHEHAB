#pragma once

#include "ufhe/api/decryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/decryptor.hpp"
#include "ufhe/secret_key.hpp"

namespace ufhe
{
class Decryptor : public api::Decryptor
{
public:
  Decryptor(const EncryptionContext &context, const SecretKey &secret_key);

  Decryptor(const Decryptor &copy) = delete;

  Decryptor &operator=(const Decryptor &assign) = delete;

  ~Decryptor() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) override
  {
    underlying().decrypt(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline int invariant_noise_budget(const api::Ciphertext &encrypted) override
  {
    return underlying().invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying());
  }

private:
  inline api::Decryptor &underlying() const { return *underlying_; }

  api::Decryptor *underlying_;
};
} // namespace ufhe
