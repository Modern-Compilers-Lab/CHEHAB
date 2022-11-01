#pragma once

#include "seal/seal.h"
#include "ufhe/api/idecryptor.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryptioncontext.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Decryptor : public api::IDecryptor
  {
  public:
    inline Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::Decryptor(context.underlying_, secret_key.underlying_))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void decrypt(const api::ICiphertext &encrypted, api::IPlaintext &destination) override
    {
      underlying_.decrypt(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline int invariant_noise_budget(const api::ICiphertext &encrypted) override
    {
      return underlying_.invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying_);
    }

  private:
    seal::Decryptor underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
