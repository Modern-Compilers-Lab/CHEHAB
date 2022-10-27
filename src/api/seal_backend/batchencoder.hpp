#pragma once

#include "../ibatchencoder.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "plaintext.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class BatchEncoder : public Implementation, public IBatchEncoder
  {
  public:
    inline BatchEncoder(const EncryptionContext &context)
      : BatchEncoder(new seal::BatchEncoder(context.underlying()), true)
    {}

    inline BatchEncoder(const BatchEncoder &copy) : BatchEncoder(copy.underlying_, false) {}

    BatchEncoder &operator=(const BatchEncoder &assign) = delete;

    inline ~BatchEncoder()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline void encode(const std::vector<std::uint64_t> &values_vector, IPlaintext &destination) const override
    {
      underlying().encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying());
    }

    inline void encode(const std::vector<std::int64_t> &values_vector, IPlaintext &destination) const override
    {
      underlying().encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying());
    }

    inline void decode(const IPlaintext &plain, std::vector<std::uint64_t> &destination) const override
    {
      underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
    }

    inline void decode(const IPlaintext &plain, std::vector<std::int64_t> &destination) const override
    {
      underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
    }

  private:
    inline BatchEncoder(seal::BatchEncoder *seal_encoder, bool is_owner)
      : underlying_(seal_encoder), is_owner_(is_owner)
    {}

    inline seal::BatchEncoder &underlying() const { return *underlying_; }

    seal::BatchEncoder *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
