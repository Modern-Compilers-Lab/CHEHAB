#include "ufhe/seal_backend/batch_encoder.hpp"
#include "seal/batchencoder.h"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
namespace seal_backend
{
  BatchEncoder::BatchEncoder(const EncryptionContext &context)
    : underlying_(std::make_shared<seal::BatchEncoder>(context.underlying()))
  {}

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
} // namespace seal_backend
} // namespace ufhe
