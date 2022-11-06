#pragma once

#include "ufhe/api/decryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/decryptor.hpp"
#include "ufhe/secret_key.hpp"
#include <memory>

namespace ufhe
{
class Decryptor : public api::Decryptor
{
public:
  Decryptor(const EncryptionContext &context, const SecretKey &secret_key);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override
  {
    underlying().decrypt(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), *dynamic_cast<Plaintext &>(destination).underlying_);
  }

  inline int invariant_noise_budget(const api::Ciphertext &encrypted) const override
  {
    return underlying().invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying());
  }

  inline const api::Decryptor &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Decryptor> underlying_;
};
} // namespace ufhe
