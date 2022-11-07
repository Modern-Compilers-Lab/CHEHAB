#pragma once

#include "ufhe/api/decryptor.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext;
class SecretKey;

class Decryptor : public api::Decryptor
{
public:
  Decryptor(const EncryptionContext &context, const SecretKey &secret_key);

  Decryptor(const Decryptor &copy);

  Decryptor &operator=(const Decryptor &assign);

  Decryptor(Decryptor &&source) = default;

  Decryptor &operator=(Decryptor &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override;

  int invariant_noise_budget(const api::Ciphertext &encrypted) const override;

  inline const api::Decryptor &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Decryptor> underlying_;
};
} // namespace ufhe
