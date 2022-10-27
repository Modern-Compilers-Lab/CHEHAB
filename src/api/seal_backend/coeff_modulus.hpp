#pragma once

#include "../icoeff_modulus.hpp"
#include "modulus.hpp"
#include "seal/seal.h"
#include <stddef.h>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class CoeffModulus : public ICoeffModulus
  {
    friend class EncryptionParameters;

  public:
    CoeffModulus(const std::vector<Modulus> &moduli);

    static inline CoeffModulus Default(size_t poly_modulus_degree)
    {
      std::vector<seal::Modulus> *moduli = new std::vector<seal::Modulus>;
      for (const seal::Modulus &modulus : seal::CoeffModulus::BFVDefault(poly_modulus_degree))
      {
        (*moduli).push_back(modulus);
      }
      return CoeffModulus(moduli, true);
    }

    inline CoeffModulus(const CoeffModulus &copy) : CoeffModulus(copy.underlying_, false) {}

    CoeffModulus &operator=(const CoeffModulus &assign) = delete;

    inline ~CoeffModulus()
    {
      if (is_owner_)
        delete underlying_;
      delete *moduli_p_;
      delete moduli_p_;
    }

    inline Backend backend() override { return Backend::seal; }

    IModulus::vector value() const override;

  private:
    inline CoeffModulus(std::vector<seal::Modulus> *seal_coeff, bool is_owner)
      : underlying_(seal_coeff), is_owner_(is_owner)
    {
      *moduli_p_ = new std::vector<Modulus>;
    }

    inline std::vector<seal::Modulus> &underlying() const { return *underlying_; }

    std::vector<seal::Modulus> *underlying_;
    bool is_owner_;

    std::vector<Modulus> **moduli_p_{new std::vector<Modulus> *};
  };
} // namespace seal_backend
} // namespace ufhe
