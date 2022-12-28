#pragma once

#include "ufhe/api/coeff_modulus.hpp"
#include "ufhe/modulus.hpp"
#include <memory>
#include <vector>

namespace ufhe
{
class CoeffModulus : public api::CoeffModulus
{
  friend class EncryptionParams;

public:
  explicit CoeffModulus(const std::vector<Modulus> &moduli);

  CoeffModulus(const CoeffModulus &copy);

  CoeffModulus &operator=(const CoeffModulus &assign);

  CoeffModulus(CoeffModulus &&source) = default;

  CoeffModulus &operator=(CoeffModulus &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  api::Modulus::vector value() const override;

  inline const api::CoeffModulus &underlying() const { return *underlying_; }

private:
  CoeffModulus();

  explicit CoeffModulus(const api::CoeffModulus &coeff_modulus);

  std::shared_ptr<api::CoeffModulus> underlying_;
  std::vector<Modulus> moduli_;
};
} // namespace ufhe
