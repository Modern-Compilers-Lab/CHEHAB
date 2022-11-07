#include "ufhe/seal_backend/scheme.hpp"
#include <stdexcept>

namespace ufhe
{
namespace seal_backend
{
  Scheme::Scheme(api::scheme_type scheme) : scheme_{scheme}
  {
    switch (scheme_)
    {
    case api::scheme_type::none:
      underlying_ = seal::scheme_type::none;
      break;

    case api::scheme_type::bfv:
      underlying_ = seal::scheme_type::bfv;
      break;

    case api::scheme_type::bgv:
      underlying_ = seal::scheme_type::bgv;
      break;

    case api::scheme_type::ckks:
      underlying_ = seal::scheme_type::ckks;
      break;

    default:
      throw std::invalid_argument("unsupported scheme");
      break;
    }
  }

  api::scheme_type Scheme::type() const
  {
    return scheme_;
  }
} // namespace seal_backend
} // namespace ufhe
