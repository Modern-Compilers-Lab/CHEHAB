#pragma once

#include "ufhe/api/coeff_modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/seal_backend/coeff_modulus.hpp"
#include <vector>

namespace ufhe
{
class CoeffModulus : public api::CoeffModulus
{
  friend class EncryptionParams;

public:
  CoeffModulus(const std::vector<Modulus> &moduli = {});

  CoeffModulus(const CoeffModulus &copy) = delete;

  CoeffModulus &operator=(const CoeffModulus &assign);

  ~CoeffModulus() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  api::Modulus::vector value() const override;

private:
  CoeffModulus(const api::CoeffModulus &coeff_modulus);

  inline api::CoeffModulus &underlying() const { return *underlying_; }

  api::CoeffModulus *underlying_;
  std::vector<Modulus> moduli_;
};
} // namespace ufhe
