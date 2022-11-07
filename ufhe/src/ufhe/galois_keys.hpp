#pragma once

#include "ufhe/api/galois_keys.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class GaloisKeys : public api::GaloisKeys
{
  friend class KeyGenerator;

public:
  explicit GaloisKeys(api::backend_type backend = Config::backend());

  GaloisKeys(const GaloisKeys &copy);

  GaloisKeys &operator=(const GaloisKeys &assign);

  GaloisKeys(GaloisKeys &&source) = default;

  GaloisKeys &operator=(GaloisKeys &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  std::size_t size() const override;

  inline const api::GaloisKeys &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::GaloisKeys> underlying_;
};
} // namespace ufhe
