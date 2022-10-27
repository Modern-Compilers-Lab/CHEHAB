#pragma once

#include "../iencryptionparameters.hpp"
#include "coeff_modulus.hpp"
#include "modulus.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class SchemeType : public ISchemeType
  {
    friend class EncryptionParameters;

  public:
    inline SchemeType(std::uint8_t scheme_id) : SchemeType(static_cast<seal::scheme_type>(scheme_id)) {}

    inline Backend backend() override { return Backend::seal; };

  private:
    inline SchemeType(seal::scheme_type seal_scheme) : underlying_(seal_scheme) {}

    inline seal::scheme_type underlying() const { return underlying_; }

    seal::scheme_type underlying_;
  };

  class EncryptionParameters : public IEncryptionParameters
  {
    friend class EncryptionContext;

  public:
    inline EncryptionParameters(const SchemeType &scheme)
      : EncryptionParameters(new seal::EncryptionParameters(scheme.underlying()), true)
    {}

    EncryptionParameters(const EncryptionParameters &copy) : EncryptionParameters(copy.underlying_, false) {}

    EncryptionParameters &operator=(const EncryptionParameters &assign) = delete;

    inline ~EncryptionParameters()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline Backend backend() override { return Backend::seal; };

    inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
    {
      underlying().set_poly_modulus_degree(poly_modulus_degree);
    }

    inline void set_coeff_modulus(const ICoeffModulus &coeff_modulus) override
    {
      underlying().set_coeff_modulus(dynamic_cast<const CoeffModulus &>(coeff_modulus).underlying());
    }

    inline void set_plain_modulus(const IModulus &plain_modulus) override
    {
      underlying().set_plain_modulus(dynamic_cast<const Modulus &>(plain_modulus).underlying());
    }

    inline const ISchemeType &scheme() const override { return scheme_; }

    inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

    inline const ICoeffModulus &coeff_modulus() const override { return coeff_modulus_; }

    inline const IModulus &plain_modulus() const override { return plain_modulus_; }

  private:
    EncryptionParameters(seal::EncryptionParameters *seal_params, bool is_owner)
      : underlying_(seal_params), is_owner_(is_owner), scheme_(underlying().scheme()),
        coeff_modulus_(const_cast<std::vector<seal::Modulus> *>(&underlying().coeff_modulus()), false),
        plain_modulus_(const_cast<seal::Modulus *>(&underlying().plain_modulus()), false)
    {}

    inline seal::EncryptionParameters &underlying() const { return *underlying_; }

    seal::EncryptionParameters *underlying_;
    bool is_owner_;

    SchemeType scheme_;
    CoeffModulus coeff_modulus_;
    Modulus plain_modulus_;
  };
} // namespace seal_backend
} // namespace ufhe
