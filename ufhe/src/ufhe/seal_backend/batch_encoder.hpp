#pragma once

#include "seal/seal.h"
#include "ufhe/api/batch_encoder.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class BatchEncoder : public api::BatchEncoder
  {
  public:
    BatchEncoder(const EncryptionContext &context)
      : underlying_(std::make_shared<seal::BatchEncoder>(context.underlying()))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline std::size_t slot_count() const override { return underlying().slot_count(); }

    inline void encode(const std::vector<std::uint64_t> &values_vector, api::Plaintext &destination) const override
    {
      underlying().encode(values_vector, *dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void encode(const std::vector<std::int64_t> &values_vector, api::Plaintext &destination) const override
    {
      underlying().encode(values_vector, *dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void decode(const api::Plaintext &plain, std::vector<std::uint64_t> &destination) const override
    {
      underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
    }

    inline void decode(const api::Plaintext &plain, std::vector<std::int64_t> &destination) const override
    {
      underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
    }

    inline const seal::BatchEncoder &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::BatchEncoder> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
