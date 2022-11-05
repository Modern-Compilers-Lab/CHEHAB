#pragma once

#include "seal/seal.h"
#include "ufhe/api/decryptor.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/secret_key.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class Decryptor : public api::Decryptor
  {
  public:
    Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(std::make_shared<seal::Decryptor>(context.underlying(), secret_key.underlying()))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override
    {
      underlying_->decrypt(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), *dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline int invariant_noise_budget(const api::Ciphertext &encrypted) const override
    {
      return underlying_->invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying());
    }

    inline const seal::Decryptor &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Decryptor> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
