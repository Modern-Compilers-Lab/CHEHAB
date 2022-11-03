#pragma once

#include "ufhe/api/relin_keys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"

namespace ufhe
{
class RelinKeys : public api::RelinKeys
{
  friend class KeyGenerator;

public:
  RelinKeys();

  RelinKeys(const RelinKeys &copy) = delete;

  RelinKeys &operator=(const RelinKeys &assign) = delete;

  ~RelinKeys() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t size() const override { return underlying().size(); }

private:
  inline api::RelinKeys &underlying() const { return *underlying_; }

  api::RelinKeys *underlying_;
};
} // namespace ufhe
