#include "ufhe/seal_backend/coeff_modulus.hpp"
#include "seal/modulus.h"

namespace ufhe
{
namespace seal_backend
{
  CoeffModulus::CoeffModulus(const std::vector<Modulus> &moduli)
    : underlying_(std::make_shared<std::vector<seal::Modulus>>()), moduli_(moduli)
  {
    underlying_->reserve(moduli_.size());
    for (const Modulus &modulus : moduli)
      underlying_->push_back(modulus.underlying());
  }

  api::Modulus::vector CoeffModulus::value() const
  {
    api::Modulus::vector moduli_wrappers(moduli_.begin(), moduli_.end());
    return moduli_wrappers;
  }

  CoeffModulus::CoeffModulus(const std::vector<seal::Modulus> &moduli)
    : underlying_(std::shared_ptr<std::vector<seal::Modulus>>(
        &const_cast<std::vector<seal::Modulus> &>(moduli), [](std::vector<seal::Modulus> *) {})),
      moduli_()
  {
    moduli_.reserve(underlying_->size());
    for (const seal::Modulus &seal_modulus : *underlying_)
      moduli_.push_back(Modulus(seal_modulus));
  }
} // namespace seal_backend
} // namespace ufhe
