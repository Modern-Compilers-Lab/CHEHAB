#pragma once

#include "ufhe/api/encryption_params.hpp"
#include "ufhe/coeff_modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/scheme.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"

namespace ufhe
{
class EncryptionParams : public api::EncryptionParams
{
  friend class EncryptionContext;

public:
  EncryptionParams(const Scheme &scheme);

  EncryptionParams(const EncryptionParams &copy) = delete;

  EncryptionParams &operator=(const EncryptionParams &assign) = delete;

  ~EncryptionParams()
  {
    delete underlying_;
    delete coeff_modulus_;
    delete plain_modulus_;
  }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void set_poly_modulus_degree(std::size_t poly_modulus_degree) override
  {
    underlying().set_poly_modulus_degree(poly_modulus_degree);
  }

  inline void set_coeff_modulus(const api::CoeffModulus &coeff_modulus) override
  {
    underlying().set_coeff_modulus(dynamic_cast<const CoeffModulus &>(coeff_modulus).underlying());
  }

  inline void set_plain_modulus(const api::Modulus &plain_modulus) override
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
  inline api::EncryptionParams &underlying() const { return *underlying_; }

  api::EncryptionParams *underlying_;
  Scheme scheme_;
  CoeffModulus *coeff_modulus_;
  Modulus *plain_modulus_;
};
} // namespace ufhe
