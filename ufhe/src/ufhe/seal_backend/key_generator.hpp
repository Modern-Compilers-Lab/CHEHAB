#pragma once

#include "seal/seal.h"
#include "ufhe/api/key_generator.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include "ufhe/seal_backend/public_key.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"
#include "ufhe/seal_backend/secret_key.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class KeyGenerator : public api::KeyGenerator
  {
  public:
    KeyGenerator(const EncryptionContext &context)
      : underlying_(std::make_shared<seal::KeyGenerator>(context.underlying())),
        secret_key_(SecretKey(underlying().secret_key()))
    {}

    KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
      : underlying_(std::make_shared<seal::KeyGenerator>(context.underlying(), secret_key.underlying())),
        secret_key_(SecretKey(underlying().secret_key()))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline const SecretKey &secret_key() const override { return secret_key_; }

    inline void create_public_key(api::PublicKey &destination) const override
    {
      underlying().create_public_key(*dynamic_cast<PublicKey &>(destination).underlying_);
    }

    inline void create_relin_keys(api::RelinKeys &destination) const override
    {
      underlying_->create_relin_keys(*dynamic_cast<RelinKeys &>(destination).underlying_);
    }

    inline void create_galois_keys(api::GaloisKeys &destination) const override
    {
      underlying_->create_galois_keys(*dynamic_cast<GaloisKeys &>(destination).underlying_);
    }

    inline const seal::KeyGenerator &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::KeyGenerator> underlying_;
    SecretKey secret_key_;
  };
} // namespace seal_backend
} // namespace ufhe
