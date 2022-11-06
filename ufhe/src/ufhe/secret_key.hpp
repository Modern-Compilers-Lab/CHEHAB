#pragma once

#include "ufhe/api/secret_key.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/secret_key.hpp"
#include <memory>

namespace ufhe
{
class SecretKey : public api::SecretKey
{
  friend class KeyGenerator;

public:
  SecretKey();

  SecretKey(const SecretKey &copy);

  SecretKey &operator=(const SecretKey &assign);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline const api::SecretKey &underlying() const { return *underlying_; }

private:
  explicit SecretKey(const api::SecretKey &secret_key);

  std::shared_ptr<api::SecretKey> underlying_;
};
} // namespace ufhe
