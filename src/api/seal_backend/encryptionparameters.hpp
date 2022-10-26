#pragma once

#include "../iencryptionparameters.hpp"
#include "modulus.hpp"
#include "seal/seal.h"

namespace ufhe
{
class EncryptionParameters;
class SchemeType;

namespace seal_backend
{
  class SchemeType : public ISchemeType
  {
    friend class ufhe::SchemeType;

  public:
    inline SchemeType(std::uint8_t scheme_id) : scheme_(static_cast<seal::scheme_type>(scheme_id)) {}

    inline Backend backend() override { return Backend::seal; };

    inline operator seal::scheme_type() const { return scheme_; }

  private:
    seal::scheme_type scheme_;
  };

  class EncryptionParameters : public IEncryptionParameters
  {
    friend class EncryptionParameters;

  public:
    inline EncryptionParameters(const ufhe::ISchemeType &scheme)
      : params_(seal::EncryptionParameters(dynamic_cast<const SchemeType &>(scheme))),
        scheme_type_(dynamic_cast<const SchemeType &>(scheme))
    {}

    inline operator const seal::EncryptionParameters &() const { return params_; }

    inline Backend backend() override { return Backend::seal; };

    inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
    {
      params_.set_poly_modulus_degree(poly_modulus_degree);
    }

    void set_coeff_modulus(const IModulus::vector &coeff_modulus) override;

    inline void set_plain_modulus(const IModulus &plain_modulus) override
    {
      plain_modulus_ = dynamic_cast<const Modulus &>(plain_modulus);
      params_.set_plain_modulus(plain_modulus_);
    }

    inline const ISchemeType &scheme() const override { return scheme_type_; }

    inline std::size_t poly_modulus_degree() const override { return params_.poly_modulus_degree(); }

    inline const IModulus::vector &coeff_modulus() const override { return coeff_modulus_; }

    inline const IModulus &plain_modulus() const override { return plain_modulus_; }

  private:
    seal::EncryptionParameters params_;
    SchemeType scheme_type_;
    IModulus::vector coeff_modulus_{};
    Modulus plain_modulus_{0};
  };
} // namespace seal_backend
} // namespace ufhe
