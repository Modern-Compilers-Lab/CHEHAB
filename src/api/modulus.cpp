#include "modulus.hpp"
#include "api.hpp"
#include "seal_backend/modulus.hpp"
#include <stdexcept>

namespace api
{
std::unique_ptr<Modulus> Modulus::create(Backend backend, std::uint64_t value)
{
  if (backend == Backend::none)
    backend = API::default_backend();
  std::unique_ptr<Modulus> ptr;
  switch (backend)
  {
  case Backend::seal:
    ptr = std::unique_ptr<seal_backend::Modulus>(new seal_backend::Modulus());
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  ptr->init(value);
  return ptr;
}
} // namespace api
