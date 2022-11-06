#include "ufhe/coeff_modulus.hpp"

namespace ufhe
{
CoeffModulus::CoeffModulus(const std::vector<Modulus> &moduli) : moduli_(moduli)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
  {
    std::vector<seal_backend::Modulus> underlying_moduli;
    underlying_moduli.reserve(moduli_.size());
    for (const Modulus &modulus : moduli)
      underlying_moduli.push_back(dynamic_cast<const seal_backend::Modulus &>(modulus.underlying()));
    underlying_ = std::make_shared<seal_backend::CoeffModulus>(underlying_moduli);
    break;
  }

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

CoeffModulus::CoeffModulus(const api::CoeffModulus &coeff_modulus)
  : underlying_(
      std::shared_ptr<api::CoeffModulus>(&const_cast<api::CoeffModulus &>(coeff_modulus), [](api::CoeffModulus *) {})),
    moduli_({})
{
  api::Modulus::vector underlying_moduli(underlying().value());
  moduli_.reserve(underlying_moduli.size());
  for (const api::Modulus &modulus : underlying_moduli)
    moduli_.push_back(Modulus(modulus));
}

api::Modulus::vector CoeffModulus::value() const
{
  api::Modulus::vector moduli_wrappers(moduli_.begin(), moduli_.end());
  return moduli_wrappers;
}

} // namespace ufhe
