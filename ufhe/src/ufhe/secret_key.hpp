#pragma once

#include "ufhe/api/secret_key.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class SecretKey : public api::SecretKey
{
  friend class KeyGenerator;

public:
  explicit SecretKey(api::backend_type backend = Config::backend());

  SecretKey(const SecretKey &copy);

  SecretKey &operator=(const SecretKey &assign);

  SecretKey(SecretKey &&source) = default;

  SecretKey &operator=(SecretKey &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline const api::SecretKey &underlying() const { return *underlying_; }

private:
  explicit SecretKey(const api::SecretKey &secret_key);

  std::shared_ptr<api::SecretKey> underlying_;
};
} // namespace ufhe
