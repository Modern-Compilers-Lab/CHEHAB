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

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override
    {
      check_strict_compatibility(encrypted);
      check_strict_compatibility(destination);
      underlying_->decrypt(
        static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Plaintext &>(destination).underlying_);
    }

    inline int invariant_noise_budget(const api::Ciphertext &encrypted) const override
    {
      check_strict_compatibility(encrypted);
      return underlying_->invariant_noise_budget(static_cast<const Ciphertext &>(encrypted).underlying());
    }

    inline const seal::Decryptor &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Decryptor> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
