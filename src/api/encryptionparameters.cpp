#include "encryptionparameters.hpp"
#include "seal_backend/encryptionparameters.hpp"
#include <stdexcept>

namespace ufhe
{

SchemeType::ptr SchemeType::create(Backend backend, std::uint8_t scheme_id)
{
  if (backend == Backend::none)
    backend = API::default_backend();
  SchemeType::ptr ptr;
  switch (backend)
  {
  case Backend::seal:
    ptr = std::make_unique<seal_backend::SchemeType>(scheme_id);
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  return ptr;
}

EncryptionParameters::ptr EncryptionParameters::create(Backend backend, const SchemeType &scheme)
{
  if (backend == Backend::none)
    backend = API::default_backend();
  EncryptionParameters::ptr ptr;
  switch (backend)
  {
  case Backend::seal:
    ptr = std::make_unique<seal_backend::EncryptionParameters>(scheme);
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
  return ptr;
}
} // namespace ufhe
