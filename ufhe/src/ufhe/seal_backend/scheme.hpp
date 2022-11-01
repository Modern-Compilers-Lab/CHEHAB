#pragma once

#include "seal/seal.h"
#include "ufhe/api/ischeme.hpp"
#include <stdexcept>

namespace ufhe
{
namespace seal_backend
{

  class Scheme : public api::IScheme
  {
    friend class EncryptionParameters;

  public:
    inline Scheme(api::scheme_type scheme)
    {
      scheme_ = scheme;
      switch (scheme_)
      {
      case api::scheme_type::none:
        underlying_ = seal::scheme_type::none;
        break;

      case api::scheme_type::bfv:
        underlying_ = seal::scheme_type::bfv;
        break;

      case api::scheme_type::bgv:
        underlying_ = seal::scheme_type::bgv;
        break;

      case api::scheme_type::ckks:
        underlying_ = seal::scheme_type::ckks;
        break;

      default:
        throw std::invalid_argument("unsupported scheme");
        break;
      }
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::scheme_type type() const override { return scheme_; }

  private:
    inline Scheme(seal::scheme_type seal_scheme) : underlying_(seal_scheme) {}

    seal::scheme_type underlying_;
    api::scheme_type scheme_;
  };
} // namespace seal_backend
} // namespace ufhe
