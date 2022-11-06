#pragma once

#include "seal/seal.h"
#include "ufhe/api/encryption_params.hpp"
#include "ufhe/seal_backend/coeff_modulus.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include "ufhe/seal_backend/scheme.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class EncryptionParams : public api::EncryptionParams
  {
  public:
    EncryptionParams(const Scheme &scheme)
      : underlying_(std::make_shared<seal::EncryptionParameters>(scheme.underlying())), scheme_(scheme),
        coeff_modulus_(underlying().coeff_modulus()), plain_modulus_(underlying().plain_modulus())
    {}

    EncryptionParams(const EncryptionParams &copy)
      : underlying_(std::make_shared<seal::EncryptionParameters>(copy.underlying())), scheme_(copy.scheme_),
        coeff_modulus_(underlying().coeff_modulus()), plain_modulus_(underlying().plain_modulus())
    {}

    EncryptionParams &operator=(const EncryptionParams &assign)
    {
      underlying_ = std::make_shared<seal::EncryptionParameters>(assign.underlying());
      scheme_ = Scheme(assign.scheme_);
      coeff_modulus_ = CoeffModulus(underlying().coeff_modulus());
      plain_modulus_ = Modulus(underlying().plain_modulus());
      return *this;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
    {
      underlying_->set_poly_modulus_degree(poly_modulus_degree);
    }

    inline void set_coeff_modulus(const api::CoeffModulus &coeff_modulus) override
    {
      check_strict_compatibility(coeff_modulus);
      underlying_->set_coeff_modulus(static_cast<const CoeffModulus &>(coeff_modulus).underlying());
    }

    inline void set_plain_modulus(const api::Modulus &plain_modulus) override
    {
      check_strict_compatibility(plain_modulus);
      underlying_->set_plain_modulus(static_cast<const Modulus &>(plain_modulus).underlying());
    }

    inline const Scheme &scheme() const override { return scheme_; }

    inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

    inline const CoeffModulus &coeff_modulus() const override { return coeff_modulus_; }

    inline const Modulus &plain_modulus() const override { return plain_modulus_; }

    inline const seal::EncryptionParameters &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::EncryptionParameters> underlying_;
    Scheme scheme_;
    CoeffModulus coeff_modulus_;
    Modulus plain_modulus_;
  };
} // namespace seal_backend
} // namespace ufhe
