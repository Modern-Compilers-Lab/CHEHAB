#pragma once

#include "ufhe/api/scheme.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/scheme.hpp"

namespace ufhe
{
class Scheme : public api::Scheme
{
  friend class EncryptionParams;

public:
  Scheme(api::scheme_type scheme);

  Scheme(const Scheme &copy);

  Scheme &operator=(const Scheme &assign) = delete;

  ~Scheme() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::scheme_type type() const override { return underlying().type(); }

private:
  inline api::Scheme &underlying() const { return *underlying_; }

  api::Scheme *underlying_;
};
} // namespace ufhe
