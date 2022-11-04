#pragma once

#include "seal_backend/key_generator.hpp"
#include "ufhe/api/key_generator.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/public_key.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/secret_key.hpp"

namespace ufhe
{
class KeyGenerator : public api::KeyGenerator
{
public:
  KeyGenerator(EncryptionContext &context);

  KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key);

  KeyGenerator(const KeyGenerator &copy) = delete;

  KeyGenerator &operator=(const KeyGenerator &assign) = delete;

  ~KeyGenerator()
  {
    delete underlying_;
    delete secret_key_;
  }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline const SecretKey &secret_key() const override { return *secret_key_; }

  inline void create_public_key(api::PublicKey &destination) const override
  {
    underlying().create_public_key(dynamic_cast<PublicKey &>(destination).underlying());
  }

  inline void create_relin_keys(api::RelinKeys &destination) override
  {
    underlying().create_relin_keys(dynamic_cast<RelinKeys &>(destination).underlying());
  }

  inline void create_galois_keys(api::GaloisKeys &destination) override
  {
    underlying().create_galois_keys(dynamic_cast<GaloisKeys &>(destination).underlying());
  }

private:
  inline api::KeyGenerator &underlying() const { return *underlying_; }

  api::KeyGenerator *underlying_;
  SecretKey *secret_key_;
};
} // namespace ufhe
