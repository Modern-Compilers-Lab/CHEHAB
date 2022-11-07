#pragma once

#include "ufhe/api/key_generator.hpp"
#include "ufhe/secret_key.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext;

class KeyGenerator : public api::KeyGenerator
{
public:
  explicit KeyGenerator(EncryptionContext &context);

  KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key);

  KeyGenerator(const KeyGenerator &copy);

  KeyGenerator &operator=(const KeyGenerator &assign);

  KeyGenerator(KeyGenerator &&source) = default;

  KeyGenerator &operator=(KeyGenerator &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  const SecretKey &secret_key() const override;

  void create_public_key(api::PublicKey &destination) const override;

  void create_relin_keys(api::RelinKeys &destination) const override;

  void create_galois_keys(api::GaloisKeys &destination) const override;

  inline const api::KeyGenerator &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::KeyGenerator> underlying_;
  SecretKey secret_key_{};
};
} // namespace ufhe
