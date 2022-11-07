#include "ufhe/batch_encoder.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/batch_encoder.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"

namespace ufhe
{
BatchEncoder::BatchEncoder(const EncryptionContext &context)
{
  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::BatchEncoder>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

std::size_t BatchEncoder::slot_count() const
{
  return underlying().slot_count();
}

void BatchEncoder::encode(const std::vector<std::uint64_t> &values_vector, api::Plaintext &destination) const
{
  underlying().encode(values_vector, *safe_static_cast<Plaintext &>(destination).underlying_);
}

void BatchEncoder::encode(const std::vector<std::int64_t> &values_vector, api::Plaintext &destination) const
{
  underlying().encode(values_vector, *safe_static_cast<Plaintext &>(destination).underlying_);
}

void BatchEncoder::decode(const api::Plaintext &plain, std::vector<std::uint64_t> &destination) const
{
  underlying().decode(safe_static_cast<const Plaintext &>(plain).underlying(), destination);
}

void BatchEncoder::decode(const api::Plaintext &plain, std::vector<std::int64_t> &destination) const
{
  underlying().decode(safe_static_cast<const Plaintext &>(plain).underlying(), destination);
}
} // namespace ufhe
