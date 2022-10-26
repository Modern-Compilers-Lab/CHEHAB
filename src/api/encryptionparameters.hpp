#pragma once

#include "iencryptionparameters.hpp"
#include "modulus.hpp"
#include "seal_backend/encryptionparameters.hpp"

namespace ufhe
{
class SchemeType : public ISchemeType
{
  friend class EncryptionParameters;

public:
  inline SchemeType(Backend backend, std::uint8_t scheme_id)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::SchemeType(scheme_id);
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline SchemeType(std::uint8_t scheme_id) : SchemeType(Backend::none, scheme_id) {}

  ~SchemeType() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

private:
  inline ISchemeType &underlying() const { return *underlying_; }

  ISchemeType *underlying_;
};

class EncryptionParameters : public IEncryptionParameters
{
public:
  inline EncryptionParameters(Backend backend, const ufhe::ISchemeType &scheme)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::EncryptionParameters(dynamic_cast<const SchemeType &>(scheme).underlying());
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline EncryptionParameters(const ufhe::ISchemeType &scheme) : EncryptionParameters(Backend::none, scheme) {}

  ~EncryptionParameters() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

  inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
  {
    underlying().set_poly_modulus_degree(poly_modulus_degree);
  }

  void set_coeff_modulus(const IModulus::vector &coeff_modulus) override;

  inline void set_plain_modulus(const IModulus &plain_modulus) override
  {
    underlying().set_plain_modulus(dynamic_cast<const Modulus &>(plain_modulus).underlying());
  }

  inline const ISchemeType &scheme() const override { return underlying().scheme(); }

  inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

  inline IModulus::vector coeff_modulus() override { return underlying().coeff_modulus(); };

  inline const IModulus &plain_modulus() const override { return underlying().plain_modulus(); }

private:
  inline IEncryptionParameters &underlying() const { return *underlying_; }

  IEncryptionParameters *underlying_;
};
} // namespace ufhe
