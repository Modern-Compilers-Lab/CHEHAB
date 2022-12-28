#include "ufhe/encryption_context.hpp"
#include "ufhe/encryption_params.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"

namespace ufhe
{
EncryptionContext::EncryptionContext(const EncryptionParams &params)
{
  switch (params.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionContext>(
      static_cast<const seal_backend::EncryptionParams &>(params.underlying()));
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
