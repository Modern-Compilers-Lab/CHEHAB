#include "encryptionparameters.hpp"

namespace ufhe
{
namespace seal_backend
{
  void EncryptionParameters::set_coeff_modulus(const ufhe::Modulus::vector &coeff_modulus)
  {
    coeff_modulus_.reserve(coeff_modulus.size());
    for (const auto &e : coeff_modulus)
      coeff_modulus_.push_back(e->clone());
    std::vector<seal::Modulus> v;
    v.reserve(coeff_modulus.size());
    for (const auto &e : coeff_modulus)
    {
      v.push_back(*dynamic_cast<Modulus *>(e.get()));
    }
    params_.set_coeff_modulus(v);
  }
} // namespace seal_backend
} // namespace ufhe
