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

  inline Backend backend() override { return underlying_->backend(); }

private:
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
      underlying_ = new seal_backend::EncryptionParameters(*dynamic_cast<const SchemeType &>(scheme).underlying_);
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline EncryptionParameters(const ufhe::ISchemeType &scheme) : EncryptionParameters(Backend::none, scheme) {}

  ~EncryptionParameters() { delete underlying_; }

  inline Backend backend() override { return underlying_->backend(); }

  inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
  {
    underlying_->set_poly_modulus_degree(poly_modulus_degree);
  }

  void set_coeff_modulus(const IModulus::vector &coeff_modulus) override;

  inline void set_plain_modulus(const IModulus &plain_modulus) override
  {
    underlying_->set_plain_modulus(*dynamic_cast<const Modulus &>(plain_modulus).underlying_);
  }

  inline const ISchemeType &scheme() const override { return underlying_->scheme(); }

  inline std::size_t poly_modulus_degree() const override { return underlying_->poly_modulus_degree(); }

  inline const IModulus::vector &coeff_modulus() const override { return underlying_->coeff_modulus(); };

  inline const IModulus &plain_modulus() const override { return underlying_->plain_modulus(); }

private:
  IEncryptionParameters *underlying_;
};
} // namespace ufhe
