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

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(destination);
    underlying().decrypt(
      static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Plaintext &>(destination).underlying_);
  }

  inline int invariant_noise_budget(const api::Ciphertext &encrypted) const override
  {
    check_strict_compatibility(encrypted);
    return underlying().invariant_noise_budget(static_cast<const Ciphertext &>(encrypted).underlying());
  }

  inline const api::Decryptor &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Decryptor> underlying_;
};
} // namespace ufhe
