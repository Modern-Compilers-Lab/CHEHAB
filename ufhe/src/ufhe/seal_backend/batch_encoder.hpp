#pragma once

#include "seal/seal.h"
#include "ufhe/api/batch_encoder.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
namespace seal_backend
{
  class BatchEncoder : public api::BatchEncoder
  {
  public:
    BatchEncoder(const EncryptionContext &context) : underlying_(seal::BatchEncoder(context.underlying_)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline std::size_t slot_count() override { return underlying_.slot_count(); }

    inline void encode(const std::vector<std::uint64_t> &values_vector, api::Plaintext &destination) const override
    {
      underlying_.encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void encode(const std::vector<std::int64_t> &values_vector, api::Plaintext &destination) const override
    {
      underlying_.encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void decode(const api::Plaintext &plain, std::vector<std::uint64_t> &destination) const override
    {
      underlying_.decode(dynamic_cast<const Plaintext &>(plain).underlying_, destination);
    }

    inline void decode(const api::Plaintext &plain, std::vector<std::int64_t> &destination) const override
    {
      underlying_.decode(dynamic_cast<const Plaintext &>(plain).underlying_, destination);
    }

  private:
    seal::BatchEncoder underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
