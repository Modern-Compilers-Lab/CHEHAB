#include "modulus.hpp"
#include "seal_backend/modulus.hpp"
#include <stdexcept>

namespace ufhe
{
std::unique_ptr<Modulus> Modulus::create(Backend backend, std::uint64_t value)
{
  if (backend == Backend::none)
    backend = API::default_backend();
  std::unique_ptr<Modulus> ptr;
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
