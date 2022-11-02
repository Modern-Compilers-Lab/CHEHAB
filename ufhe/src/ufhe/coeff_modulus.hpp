#pragma once

#include "ufhe/api/icoeff_modulus.hpp"
#include "ufhe/config.hpp"
#include "ufhe/modulus.hpp"
#include "ufhe/seal_backend/coeff_modulus.hpp"
#include <vector>

namespace ufhe
{
class CoeffModulus : public api::ICoeffModulus
{
  friend class EncryptionParameters;

public:
  CoeffModulus(const std::vector<Modulus> &moduli = {});

  CoeffModulus(const CoeffModulus &copy) = delete;

  CoeffModulus &operator=(const CoeffModulus &assign)
  {
    delete underlying_;
    moduli_ = assign.moduli_;

    switch (assign.backend())
    {
    case api::backend_type::seal:
      underlying_ =
        new seal_backend::CoeffModulus(dynamic_cast<const seal_backend::CoeffModulus &>(assign.underlying()));
      break;

    default:
      throw std::logic_error("instance with unknown backend");
      break;
    }
    return *this;
  }

  ~CoeffModulus() { delete underlying_; }

  inline api::backend_type backend() const override { return api::backend_type::seal; }

  api::IModulus::vector value() const override;

private:
  CoeffModulus(const api::ICoeffModulus &coeff_modulus);

  inline ICoeffModulus &underlying() const { return *underlying_; }

  api::ICoeffModulus *underlying_;
  std::vector<Modulus> moduli_;
};
} // namespace ufhe
