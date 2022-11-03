#include "ufhe/batch_encoder.hpp"

namespace ufhe
{
BatchEncoder::BatchEncoder(const EncryptionContext &context)
{
  switch (Config::backend())
  {
  case api::backend_type::seal:
    underlying_ =
      new seal_backend::BatchEncoder(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
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
