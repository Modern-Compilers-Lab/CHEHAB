#pragma once

#include "ufhe/api/batch_encoder.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/batch_encoder.hpp"
#include <memory>

namespace ufhe
{
class BatchEncoder : public api::BatchEncoder
{
public:
  BatchEncoder(const EncryptionContext &context);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline std::size_t slot_count() const override { return underlying().slot_count(); }

  inline void encode(const std::vector<std::uint64_t> &values_vector, api::Plaintext &destination) const override
  {
    check_strict_compatibility(destination);
    underlying().encode(values_vector, *static_cast<Plaintext &>(destination).underlying_);
  }

  inline void encode(const std::vector<std::int64_t> &values_vector, api::Plaintext &destination) const override
  {
    check_strict_compatibility(destination);
    underlying().encode(values_vector, *static_cast<Plaintext &>(destination).underlying_);
  }

  inline void decode(const api::Plaintext &plain, std::vector<std::uint64_t> &destination) const override
  {
    check_strict_compatibility(plain);
    underlying().decode(static_cast<const Plaintext &>(plain).underlying(), destination);
  }

  inline void decode(const api::Plaintext &plain, std::vector<std::int64_t> &destination) const override
  {
    check_strict_compatibility(plain);
    underlying().decode(static_cast<const Plaintext &>(plain).underlying(), destination);
  }

  inline const api::BatchEncoder &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::BatchEncoder> underlying_;
};
} // namespace ufhe
