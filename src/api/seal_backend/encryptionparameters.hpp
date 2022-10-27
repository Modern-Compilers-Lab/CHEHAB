#pragma once

#include "../iencryptionparameters.hpp"
#include "coeff_modulus.hpp"
#include "implementation.hpp"
#include "modulus.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class SchemeType : public Implementation, public ISchemeType
  {
    friend class EncryptionParameters;

  public:
    inline SchemeType(std::uint8_t scheme_id) : SchemeType(static_cast<seal::scheme_type>(scheme_id)) {}

    inline Backend backend() const override { return Backend::seal; }

  private:
    inline SchemeType(seal::scheme_type seal_scheme) : underlying_(seal_scheme) {}

    seal::scheme_type underlying_;
  };

  class EncryptionParameters : public Implementation, public IEncryptionParameters
  {
    friend class EncryptionContext;

  public:
    inline EncryptionParameters(const SchemeType &scheme)
      : underlying_(seal::EncryptionParameters(scheme.underlying_)), scheme_(scheme),
        coeff_modulus_p_(new CoeffModulus(underlying_.coeff_modulus())),
        plain_modulus_p_(new Modulus(underlying_.plain_modulus()))
    {}

    EncryptionParameters(const EncryptionParameters &copy) = delete;

    EncryptionParameters &operator=(const EncryptionParameters &assign) = delete;

    inline ~EncryptionParameters()
    {
      delete coeff_modulus_p_;
      delete plain_modulus_p_;
    }

    inline Backend backend() const override { return Backend::seal; }

    inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
    {
      underlying_.set_poly_modulus_degree(poly_modulus_degree);
    }

    inline void set_coeff_modulus(const ICoeffModulus &coeff_modulus) override
    {
      underlying_.set_coeff_modulus(dynamic_cast<const CoeffModulus &>(coeff_modulus).underlying_);
    }

    inline void set_plain_modulus(const IModulus &plain_modulus) override
    {
      underlying_.set_plain_modulus(dynamic_cast<const Modulus &>(plain_modulus).underlying_);
    }

    inline const ISchemeType &scheme() const override { return scheme_; }

    inline std::size_t poly_modulus_degree() const override { return underlying_.poly_modulus_degree(); }

    inline const ICoeffModulus &coeff_modulus() const override
    {
      *coeff_modulus_p_ = CoeffModulus(underlying_.coeff_modulus());
      return *coeff_modulus_p_;
    }

    inline const IModulus &plain_modulus() const override
    {
      *plain_modulus_p_ = Modulus(underlying_.plain_modulus());
      return *plain_modulus_p_;
    }

  private:
    seal::EncryptionParameters underlying_;
    SchemeType scheme_;
    CoeffModulus *coeff_modulus_p_;
    Modulus *plain_modulus_p_;
  };
} // namespace seal_backend
} // namespace ufhe
