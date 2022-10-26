#include "encryptionparameters.hpp"

namespace ufhe
{
namespace seal_backend
{
  void EncryptionParameters::set_coeff_modulus(const IModulus::vector &coeff_modulus)
  {
    coeff_modulus_.reserve(coeff_modulus.size());
    std::vector<seal::Modulus> seal_coeff_modulus;
    seal_coeff_modulus.reserve(coeff_modulus.size());
    for (const IModulus &e : coeff_modulus)
    {
      Modulus modulus = dynamic_cast<const Modulus &>(e);
      coeff_modulus_.push_back(modulus);
      seal_coeff_modulus.push_back(modulus);
    }
    params_.set_coeff_modulus(seal_coeff_modulus);
  }
} // namespace seal_backend
} // namespace ufhe
