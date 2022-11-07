#pragma once

#include "ufhe/api/encryption_params.hpp"
#include "ufhe/coeff_modulus.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/scheme.hpp"
#include <memory>

namespace ufhe
{
class EncryptionParams : public api::EncryptionParams
{
  friend class EncryptionContext;

public:
  explicit EncryptionParams(const Scheme &scheme);

  EncryptionParams(const EncryptionParams &copy);

  EncryptionParams &operator=(const EncryptionParams &assign);

  EncryptionParams(EncryptionParams &&source) = default;

  EncryptionParams &operator=(EncryptionParams &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  void set_poly_modulus_degree(std::size_t poly_modulus_degree) override;

  void set_coeff_modulus(const api::CoeffModulus &coeff_modulus) override;

  void set_plain_modulus(const api::Modulus &plain_modulus) override;

  const Scheme &scheme() const override;

  std::size_t poly_modulus_degree() const override;

  const CoeffModulus &coeff_modulus() const override;

  const Modulus &plain_modulus() const override;

  inline const api::EncryptionParams &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::EncryptionParams> underlying_;
  Scheme scheme_;
  CoeffModulus coeff_modulus_{};
  Modulus plain_modulus_{};
};
} // namespace ufhe
