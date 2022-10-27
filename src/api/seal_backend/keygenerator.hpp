#pragma once

#include "../ikeygenerator.hpp"
#include "encryptioncontext.hpp"
#include "publickey.hpp"
#include "seal/seal.h"
#include "secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class KeyGenerator : public IKeyGenerator
  {
    friend class EncryptionParameters;

  public:
    inline KeyGenerator(const EncryptionContext &context) : underlying_(seal::KeyGenerator(context.underlying_)) {}

    inline KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::KeyGenerator(context.underlying_, secret_key.underlying_))
    {}

    inline Backend backend() const override { return Backend::seal; }

    inline void create_public_key(IPublicKey &destination) const override
    {
      underlying_.create_public_key(dynamic_cast<PublicKey &>(destination).underlying_);
    }

  private:
    seal::KeyGenerator underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
