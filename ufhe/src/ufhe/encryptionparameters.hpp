#pragma once

#include "ufhe/api/iencryptionparameters.hpp"
#include "ufhe/coeffmodulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/scheme.hpp"
#include "ufhe/seal_backend/encryptionparameters.hpp"

namespace ufhe
{
class EncryptionParameters : public api::IEncryptionParameters
{
public:
  EncryptionParameters(const Scheme &scheme)
    : scheme_(scheme), coeff_modulus_(new CoeffModulus()), plain_modulus_(new Modulus())
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ =
        new seal_backend::EncryptionParameters(dynamic_cast<const seal_backend::Scheme &>(scheme.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  EncryptionParameters(const EncryptionParameters &copy) = delete;

  EncryptionParameters &operator=(const EncryptionParameters &assign) = delete;

  ~EncryptionParameters() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
  {
    underlying().set_poly_modulus_degree(poly_modulus_degree);
  }

  void set_coeff_modulus(const api::ICoeffModulus &coeff_modulus) override
  {
    underlying().set_coeff_modulus(dynamic_cast<const CoeffModulus &>(coeff_modulus).underlying());
  }

  inline void set_plain_modulus(const api::IModulus &plain_modulus) override
  {
    underlying().set_plain_modulus(dynamic_cast<const Modulus &>(plain_modulus).underlying());
  }

  inline const Scheme &scheme() const override { return scheme_; }

  inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

  inline const CoeffModulus &coeff_modulus() const override
  {
    *coeff_modulus_ = CoeffModulus(underlying().coeff_modulus());
    return *coeff_modulus_;
  }

  inline const Modulus &plain_modulus() const override
  {
    *plain_modulus_ = Modulus(underlying().plain_modulus());
    return *plain_modulus_;
  }

private:
  inline IEncryptionParameters &underlying() const { return *underlying_; }

  IEncryptionParameters *underlying_;
  Scheme scheme_;
  CoeffModulus *coeff_modulus_;
  Modulus *plain_modulus_;
};
} // namespace ufhe
