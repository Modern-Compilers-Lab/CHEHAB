#pragma once

#include "ufhe/api/secret_key.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/secret_key.hpp"

namespace ufhe
{
class SecretKey : public api::SecretKey
{
  friend class Decryptor;
  friend class Encryptor;
  friend class KeyGenerator;

public:
  SecretKey();

  SecretKey(const SecretKey &copy);

  SecretKey &operator=(const SecretKey &assign) = delete;

  ~SecretKey() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  SecretKey(const api::SecretKey &isecret_key);

  inline api::SecretKey &underlying() const { return *underlying_; }

  api::SecretKey *underlying_;
};
} // namespace ufhe
