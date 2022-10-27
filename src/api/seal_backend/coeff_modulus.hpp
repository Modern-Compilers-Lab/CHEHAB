#pragma once

#include "../icoeff_modulus.hpp"
#include "implementation.hpp"
#include "modulus.hpp"
#include "seal/seal.h"
#include <stddef.h>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class CoeffModulus : public Implementation, public ICoeffModulus
  {
    friend class EncryptionParameters;

  public:
    CoeffModulus(const std::vector<Modulus> &moduli);

    static inline CoeffModulus Default(size_t poly_modulus_degree)
    {
      return CoeffModulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    }

    IModulus::vector value() const override;

  private:
    CoeffModulus(std::vector<seal::Modulus> seal_moduli);

    std::vector<seal::Modulus> underlying_{};
    std::vector<Modulus> moduli_;
  };
} // namespace seal_backend
} // namespace ufhe
