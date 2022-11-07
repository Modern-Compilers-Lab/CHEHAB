#include "ufhe/relin_keys.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"

namespace ufhe
{
RelinKeys::RelinKeys(api::backend_type backend)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::RelinKeys>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

RelinKeys::RelinKeys(const RelinKeys &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::RelinKeys>(static_cast<const seal_backend::RelinKeys &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

RelinKeys &RelinKeys::operator=(const RelinKeys &assign)
{
  switch (assign.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::RelinKeys>(static_cast<const seal_backend::RelinKeys &>(assign.underlying()));
    return *this;
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

std::size_t RelinKeys::size() const
{
  return underlying().size();
}
} // namespace ufhe
