#include "ufhe/encryption_context.hpp"

namespace ufhe
{
EncryptionContext::EncryptionContext(const EncryptionParams &params)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::EncryptionContext>(
      dynamic_cast<const seal_backend::EncryptionParams &>(params.underlying()));
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
