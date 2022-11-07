#pragma once

#include "seal/encryptionparams.h"
#include "ufhe/api/scheme.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Scheme : public api::Scheme
  {
  public:
    explicit Scheme(api::scheme_type scheme);

    Scheme(const Scheme &copy) = default;

    Scheme &operator=(const Scheme &assign) = default;

    Scheme(Scheme &&source) = default;

    Scheme &operator=(Scheme &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    api::scheme_type type() const override;

    inline const seal::scheme_type &underlying() const { return underlying_; }

  private:
    seal::scheme_type underlying_;
    api::scheme_type scheme_;
  };
} // namespace seal_backend
} // namespace ufhe
