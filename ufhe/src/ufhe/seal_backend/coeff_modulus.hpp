#pragma once

#include "seal/seal.h"
#include "ufhe/api/coeff_modulus.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include <cstddef>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class CoeffModulus : public api::CoeffModulus
  {
    friend class EncryptionParams;

  public:
    CoeffModulus(const std::vector<Modulus> &moduli = {});

    static inline CoeffModulus Default(size_t poly_modulus_degree)
    {
      return CoeffModulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    api::Modulus::vector value() const override;

  private:
    CoeffModulus(std::vector<seal::Modulus> seal_moduli);

    std::vector<seal::Modulus> underlying_{};
    std::vector<Modulus> moduli_;
  };
} // namespace seal_backend
} // namespace ufhe
