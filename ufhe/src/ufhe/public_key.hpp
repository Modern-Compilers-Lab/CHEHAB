#pragma once

#include "ufhe/api/public_key.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class PublicKey : public api::PublicKey
{
  friend class KeyGenerator;

public:
  explicit PublicKey(api::backend_type backend = Config::backend());

  PublicKey(const PublicKey &copy);

  PublicKey &operator=(const PublicKey &assign);

  PublicKey(PublicKey &&source) = default;

  PublicKey &operator=(PublicKey &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline const api::PublicKey &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::PublicKey> underlying_;
};
} // namespace ufhe
