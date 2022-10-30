#include "encryptionparameters.hpp"

namespace ufhe
{
void EncryptionParameters::set_coeff_modulus(const IModulus::vector &coeff_modulus)
{
  IModulus::vector coeff_modulus_;
  coeff_modulus_.reserve(coeff_modulus.size());
  for (const IModulus &e : coeff_modulus)
  {
    const Modulus &modulus = dynamic_cast<const Modulus &>(e);
    coeff_modulus_.push_back(*modulus.underlying_);
  }
  underlying_->set_coeff_modulus(coeff_modulus_);
}
} // namespace ufhe
