#pragma once

#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "plaintext.hpp"
#include "secretkey.hpp"

namespace api
{
class Decryptor
{
public:
  Decryptor(const EncryptionContext &context, const SecretKey &secret_key) { init(context, secret_key); }

  virtual ~Decryptor() {}

  virtual void decrypt(const Ciphertext &encrypted, Plaintext &destination) const = 0;

  virtual int invariant_noise_budget(const Ciphertext &encrypted) const = 0;

private:
  virtual void init(const EncryptionContext &context, const SecretKey &secret_key) = 0;
};
} // namespace api
