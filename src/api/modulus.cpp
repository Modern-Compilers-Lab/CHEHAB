#include "modulus.hpp"
#include "seal_backend/modulus.hpp"
#include <stdexcept>

namespace ufhe
{
Modulus::ptr Modulus::create(Backend backend, std::uint64_t value)
{
  if (backend == Backend::none)
    backend = API::default_backend();
  Modulus::ptr ptr;
  switch (backend)
  {
  case Backend::seal:
    ptr = std::make_unique<seal_backend::Modulus>(value);
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  return ptr;
}
} // namespace ufhe
