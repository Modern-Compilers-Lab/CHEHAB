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
    inline KeyGenerator(const EncryptionContext &context)
      : underlying_(seal::KeyGenerator(context.underlying_)), secret_key_p_(new SecretKey(underlying_.secret_key()))
    {}

    inline KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(seal::KeyGenerator(context.underlying_, secret_key.underlying_))
    {}

    KeyGenerator(const KeyGenerator &copy) = delete;

    KeyGenerator &operator=(const KeyGenerator &assign) = delete;

    inline ~KeyGenerator() { delete secret_key_p_; }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline const SecretKey &secret_key() const override
    {
      *secret_key_p_ = SecretKey(underlying_.secret_key());
      return *secret_key_p_;
    }

    inline void create_public_key(api::IPublicKey &destination) const override
    {
      underlying_.create_public_key(dynamic_cast<PublicKey &>(destination).underlying_);
    }

  private:
    seal::KeyGenerator underlying_;
    SecretKey *secret_key_p_;
  };
} // namespace seal_backend
} // namespace ufhe
