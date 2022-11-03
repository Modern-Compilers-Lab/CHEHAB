#pragma once

#include "seal/seal.h"
#include "ufhe/api/key_generator.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/public_key.hpp"
#include "ufhe/seal_backend/secret_key.hpp"

namespace ufhe
{
namespace seal_backend
{
  class KeyGenerator : public api::KeyGenerator
  {
    friend class EncryptionParams;

  public:
    KeyGenerator(const EncryptionContext &context)
      : underlying_(seal::KeyGenerator(context.underlying_)), secret_key_(SecretKey(underlying_.secret_key()))
    {}

    KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::KeyGenerator(context.underlying_, secret_key.underlying_)), secret_key_(secret_key)
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline const SecretKey &secret_key() const override { return secret_key_; }

    inline void create_public_key(api::PublicKey &destination) const override
    {
      underlying_.create_public_key(dynamic_cast<PublicKey &>(destination).underlying_);
    }

  private:
    seal::KeyGenerator underlying_;
    SecretKey secret_key_;
  };
} // namespace seal_backend
} // namespace ufhe
