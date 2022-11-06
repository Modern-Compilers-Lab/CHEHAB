#pragma once

#include "ufhe/api/relin_keys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"
#include <memory>

namespace ufhe
{
class RelinKeys : public api::RelinKeys
{
  friend class KeyGenerator;

public:
  RelinKeys();

  RelinKeys(const RelinKeys &copy);

  RelinKeys &operator=(const RelinKeys &assign);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline std::size_t size() const override { return underlying().size(); }

  inline const api::RelinKeys &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::RelinKeys> underlying_;
};
} // namespace ufhe
