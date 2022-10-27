#pragma once

#include "../idecryptor.hpp"
#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "plaintext.hpp"
#include "seal/seal.h"
#include "secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Decryptor : public Implementation, public IDecryptor
  {
  public:
    inline Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::Decryptor(context.underlying_, secret_key.underlying_))
    {}

    inline void decrypt(const ICiphertext &encrypted, IPlaintext &destination) override
    {
      underlying_.decrypt(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline int invariant_noise_budget(const ICiphertext &encrypted) override
    {
      return underlying_.invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying_);
    }

  private:
    seal::Decryptor underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
