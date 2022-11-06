#include "ufhe/scheme.hpp"

namespace ufhe
{
Scheme::Scheme(api::scheme_type scheme)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Scheme>(scheme);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}
} // namespace ufhe
