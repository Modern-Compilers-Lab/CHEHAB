#pragma once

#include "seal/seal.h"
#include "ufhe/api/ikeygenerator.hpp"
#include "ufhe/seal_backend/encryptioncontext.hpp"
#include "ufhe/seal_backend/publickey.hpp"
#include "ufhe/seal_backend/secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class KeyGenerator : public api::IKeyGenerator
  {
    friend class EncryptionParameters;

  public:
    KeyGenerator(const EncryptionContext &context)
      : underlying_(seal::KeyGenerator(context.underlying_)), secret_key_(SecretKey(underlying_.secret_key()))
    {}

    KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::KeyGenerator(context.underlying_, secret_key.underlying_)), secret_key_(secret_key)
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline const SecretKey &secret_key() const override { return secret_key_; }

    inline void create_public_key(api::IPublicKey &destination) const override
    {
      underlying_.create_public_key(dynamic_cast<PublicKey &>(destination).underlying_);
    }

  private:
    seal::KeyGenerator underlying_;
    SecretKey secret_key_;
  };
} // namespace seal_backend
} // namespace ufhe
