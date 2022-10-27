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
    inline KeyGenerator(const EncryptionContext &context)
      : KeyGenerator(new seal::KeyGenerator(context.underlying()), true)
    {}

    inline KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : KeyGenerator(new seal::KeyGenerator(context.underlying(), secret_key.underlying()), true)
    {}

    inline KeyGenerator(const KeyGenerator &copy) : KeyGenerator(copy.underlying_, false) {}

    KeyGenerator &operator=(const KeyGenerator &assign) = delete;

    inline ~KeyGenerator()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline Backend backend() override { return Backend::seal; };

    inline const ISecretKey &secret_key() const override { return secret_key_; }

    inline void create_public_key(IPublicKey &destination) const override
    {
      underlying().create_public_key(dynamic_cast<PublicKey &>(destination).underlying());
    }

  private:
    inline KeyGenerator(seal::KeyGenerator *seal_keygen, bool is_owner)
      : underlying_(seal_keygen), is_owner_(is_owner),
        secret_key_(const_cast<seal::SecretKey *>(&underlying().secret_key()), false)
    {}

    inline seal::KeyGenerator &underlying() const { return *underlying_; }

    seal::KeyGenerator *underlying_;
    bool is_owner_;

    SecretKey secret_key_;
  };
} // namespace seal_backend
} // namespace ufhe
