#include "ufhe/coeff_modulus.hpp"

namespace ufhe
{
CoeffModulus::CoeffModulus(const std::vector<Modulus> &moduli) : moduli_(moduli)
{
  std::vector<seal_backend::Modulus> underlying_moduli;
  underlying_moduli.reserve(moduli_.size());
  for (const Modulus &modulus : moduli)
    underlying_moduli.push_back(dynamic_cast<const seal_backend::Modulus &>(modulus.underlying()));

  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::CoeffModulus(underlying_moduli);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}
CoeffModulus::CoeffModulus(const api::ICoeffModulus &coeff_modulus) : moduli_({})
{
  api::IModulus::vector imoduli = coeff_modulus.value();
  moduli_.reserve(imoduli.size());
  for (const api::IModulus &imodulus : imoduli)
    moduli_.push_back(Modulus(imodulus));

  switch (coeff_modulus.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::CoeffModulus(dynamic_cast<const seal_backend::CoeffModulus &>(coeff_modulus));
    break;

  default:
    throw std::logic_error("instance with unknown backend");
    break;
  }
}

api::IModulus::vector CoeffModulus::value() const
{
  api::IModulus::vector moduli_wrappers(moduli_.begin(), moduli_.end());
  return moduli_wrappers;
}

} // namespace ufhe
