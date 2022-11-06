#pragma once

#include "ufhe/api/galois_keys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include <memory>

namespace ufhe
{
class GaloisKeys : public api::GaloisKeys
{
  friend class KeyGenerator;

public:
  GaloisKeys();

  GaloisKeys(const GaloisKeys &copy);

  GaloisKeys &operator=(const GaloisKeys &assign);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline std::size_t size() const override { return underlying().size(); }

  inline const api::GaloisKeys &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::GaloisKeys> underlying_;
};
} // namespace ufhe
