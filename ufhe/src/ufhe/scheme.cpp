#include "ufhe/scheme.hpp"

namespace ufhe
{
Scheme::Scheme(api::scheme_type scheme)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Scheme(scheme);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Scheme::Scheme(const Scheme &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ = new seal_backend::Scheme(dynamic_cast<const seal_backend::Scheme &>(copy.underlying()));
    break;

  default:
    throw std::logic_error("instance with unknown backend");
    break;
  }
}
} // namespace ufhe
