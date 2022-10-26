#include "encryptionparameters.hpp"

namespace ufhe
{
namespace seal_backend
{
  void EncryptionParameters::set_coeff_modulus(const IModulus::vector &coeff_modulus)
  {
    std::vector<seal::Modulus> seal_coeff_modulus;
    seal_coeff_modulus.reserve(coeff_modulus.size());
    for (const IModulus &e : coeff_modulus)
      seal_coeff_modulus.push_back(dynamic_cast<const Modulus &>(e).underlying());
    underlying().set_coeff_modulus(seal_coeff_modulus);
  }

  IModulus::vector EncryptionParameters::coeff_modulus()
  {
    const std::vector<seal::Modulus> &seal_coeff_modulus = underlying().coeff_modulus();
    coeff_modulus_holder_.reserve(seal_coeff_modulus.size());
    IModulus::vector coeff_modulus;
    coeff_modulus.reserve(seal_coeff_modulus.size());
    for (const seal::Modulus &seal_modulus : seal_coeff_modulus)
    {
      coeff_modulus_holder_.push_back(Modulus(const_cast<seal::Modulus *>(&seal_modulus), false));
      coeff_modulus.push_back(coeff_modulus_holder_.back());
    }
    return coeff_modulus;
  }
} // namespace seal_backend
} // namespace ufhe
