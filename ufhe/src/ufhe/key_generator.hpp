#pragma once

#include "seal_backend/key_generator.hpp"
#include "ufhe/api/key_generator.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/public_key.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/secret_key.hpp"
#include <memory>

namespace ufhe
{
class KeyGenerator : public api::KeyGenerator
{
public:
  KeyGenerator(EncryptionContext &context);

  KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline const SecretKey &secret_key() const override { return secret_key_; }

  inline void create_public_key(api::PublicKey &destination) const override
  {
    underlying().create_public_key(*dynamic_cast<PublicKey &>(destination).underlying_);
  }

  inline void create_relin_keys(api::RelinKeys &destination) const override
  {
    underlying().create_relin_keys(*dynamic_cast<RelinKeys &>(destination).underlying_);
  }

  inline void create_galois_keys(api::GaloisKeys &destination) const override
  {
    underlying().create_galois_keys(*dynamic_cast<GaloisKeys &>(destination).underlying_);
  }

  inline const api::KeyGenerator &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::KeyGenerator> underlying_;
  SecretKey secret_key_{};
};
} // namespace ufhe
