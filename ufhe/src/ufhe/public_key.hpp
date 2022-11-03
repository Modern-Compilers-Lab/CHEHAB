#pragma once

#include "ufhe/api/public_key.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/public_key.hpp"

namespace ufhe
{
class PublicKey : public api::PublicKey
{
  friend class Encryptor;
  friend class KeyGenerator;

public:
  PublicKey();

  PublicKey(const PublicKey &copy) = delete;

  PublicKey &operator=(const PublicKey &assign) = delete;

  ~PublicKey() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  inline api::PublicKey &underlying() const { return *underlying_; }

  api::PublicKey *underlying_;
};
} // namespace ufhe
