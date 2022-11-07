#pragma once

#include "ufhe/api/relin_keys.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class RelinKeys : public api::RelinKeys
{
  friend class KeyGenerator;

public:
  explicit RelinKeys(api::backend_type backend = Config::backend());

  RelinKeys(const RelinKeys &copy);

  RelinKeys &operator=(const RelinKeys &assign);

  RelinKeys(RelinKeys &&source) = default;

  RelinKeys &operator=(RelinKeys &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  std::size_t size() const override;

  inline const api::RelinKeys &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::RelinKeys> underlying_;
};
} // namespace ufhe
