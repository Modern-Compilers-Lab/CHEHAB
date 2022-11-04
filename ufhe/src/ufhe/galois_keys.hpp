#pragma once

#include "ufhe/api/galois_keys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"

namespace ufhe
{
class GaloisKeys : public api::GaloisKeys
{
  friend class Evaluator;
  friend class KeyGenerator;

public:
  GaloisKeys();

  GaloisKeys(const GaloisKeys &copy) = delete;

  GaloisKeys &operator=(const GaloisKeys &assign) = delete;

  ~GaloisKeys() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t size() const override { return underlying().size(); }

private:
  inline api::GaloisKeys &underlying() const { return *underlying_; }

  api::GaloisKeys *underlying_;
};
} // namespace ufhe
