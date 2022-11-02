#pragma once

#include "ufhe/api/ischeme.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/scheme.hpp"

namespace ufhe
{
class Scheme : public api::IScheme
{
  friend class EncryptionParameters;

public:
  Scheme(api::scheme_type scheme)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Scheme(scheme);
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Scheme(const Scheme &copy)
  {
    switch (copy.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Scheme(dynamic_cast<const seal_backend::Scheme &>(copy.underlying()));
      break;

    default:
      throw std::logic_error("instance with unknown backend");
      break;
    }
  }

  Scheme &operator=(const Scheme &assign) = delete;

  ~Scheme() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::scheme_type type() const override { return underlying().type(); }

private:
  inline IScheme &underlying() const { return *underlying_; }

  api::IScheme *underlying_;
};
} // namespace ufhe
