#pragma once

#include "seal/seal.h"
#include "ufhe/api/iencryptionparameters.hpp"
#include "ufhe/seal_backend/coeffmodulus.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include "ufhe/seal_backend/scheme.hpp"

namespace ufhe
{
namespace seal_backend
{
  class EncryptionParameters : public api::IEncryptionParameters
  {
    friend class EncryptionContext;

  public:
    inline EncryptionParameters(const Scheme &scheme)
      : underlying_(seal::EncryptionParameters(scheme.underlying_)), scheme_(scheme),
        coeff_modulus_(new CoeffModulus()), plain_modulus_(new Modulus())
    {}

    EncryptionParameters(const EncryptionParameters &copy) = delete;

    EncryptionParameters &operator=(const EncryptionParameters &assign) = delete;

    inline ~EncryptionParameters()
    {
      delete coeff_modulus_;
      delete plain_modulus_;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
    {
      underlying_.set_poly_modulus_degree(poly_modulus_degree);
    }

    inline void set_coeff_modulus(const api::ICoeffModulus &coeff_modulus) override
    {
      underlying_.set_coeff_modulus(dynamic_cast<const CoeffModulus &>(coeff_modulus).underlying_);
    }

    inline void set_plain_modulus(const api::IModulus &plain_modulus) override
    {
      underlying_.set_plain_modulus(dynamic_cast<const Modulus &>(plain_modulus).underlying_);
    }

    inline const Scheme &scheme() const override { return scheme_; }

    inline std::size_t poly_modulus_degree() const override { return underlying_.poly_modulus_degree(); }

    inline const CoeffModulus &coeff_modulus() const override
    {
      *coeff_modulus_ = CoeffModulus(underlying_.coeff_modulus());
      return *coeff_modulus_;
    }

    inline const Modulus &plain_modulus() const override
    {
      *plain_modulus_ = Modulus(underlying_.plain_modulus());
      return *plain_modulus_;
    }

  private:
    seal::EncryptionParameters underlying_;
    Scheme scheme_;
    CoeffModulus *coeff_modulus_;
    Modulus *plain_modulus_;
  };
} // namespace seal_backend
} // namespace ufhe
