#include "coeff_modulus.hpp"

namespace ufhe
{
namespace seal_backend
{
  CoeffModulus::CoeffModulus(const std::vector<Modulus> &moduli) : CoeffModulus(new std::vector<seal::Modulus>, true)
  {
    for (const Modulus &modulus : moduli)
      underlying().push_back(modulus.underlying());
  }

  IModulus::vector CoeffModulus::value() const
  {
    delete *moduli_p_;
    *moduli_p_ = new std::vector<Modulus>;
    std::vector<Modulus> &moduli_ = **moduli_p_;
    moduli_.reserve(underlying().size());
    for (seal::Modulus &seal_modulus : underlying())
      moduli_.push_back(Modulus(&seal_modulus, false));
    IModulus::vector moduli_wrappers;
    moduli_wrappers.reserve(moduli_.size());
    for (const Modulus &modulus : moduli_)
      moduli_wrappers.push_back(modulus);
    return moduli_wrappers;
  }
} // namespace seal_backend

} // namespace ufhe
