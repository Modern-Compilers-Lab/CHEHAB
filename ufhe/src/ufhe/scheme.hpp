#pragma once

#include "ufhe/api/scheme.hpp"
#include <memory>

namespace ufhe
{
class Scheme : public api::Scheme
{
public:
  Scheme(api::backend_type backend, api::scheme_type scheme);

  explicit Scheme(api::scheme_type scheme);

  Scheme(const Scheme &copy) = default;

  Scheme &operator=(const Scheme &assign) = default;

  Scheme(Scheme &&source) = default;

  Scheme &operator=(Scheme &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  api::scheme_type type() const override;

  inline const api::Scheme &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Scheme> underlying_;
};
} // namespace ufhe
