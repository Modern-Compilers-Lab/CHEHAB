#pragma once

#include "ufhe/api/encryption_params.hpp"
#include "ufhe/coeff_modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/scheme.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"
#include <memory>

namespace ufhe
{
class EncryptionParams : public api::EncryptionParams
{
  friend class EncryptionContext;

public:
  EncryptionParams(const Scheme &scheme);

  EncryptionParams(const EncryptionParams &copy);

  EncryptionParams &operator=(const EncryptionParams &assign);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

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

  inline const api::EncryptionParams &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::EncryptionParams> underlying_;
  Scheme scheme_;
  CoeffModulus coeff_modulus_{};
  Modulus plain_modulus_{};
};
} // namespace ufhe
