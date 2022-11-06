#pragma once

#include "ufhe/api/coeff_modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/seal_backend/coeff_modulus.hpp"
#include <memory>
#include <vector>

namespace ufhe
{
class CoeffModulus : public api::CoeffModulus
{
  friend class EncryptionParams;

public:
  CoeffModulus(const std::vector<Modulus> &moduli);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  api::Modulus::vector value() const override;

  inline const api::CoeffModulus &underlying() const { return *underlying_; }

private:
  CoeffModulus() : moduli_({}) {}

  explicit CoeffModulus(const api::CoeffModulus &coeff_modulus);

  std::shared_ptr<api::CoeffModulus> underlying_;
  std::vector<Modulus> moduli_;
};
} // namespace ufhe
