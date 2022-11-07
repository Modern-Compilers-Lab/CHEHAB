#pragma once

#include "ufhe/api/batch_encoder.hpp"
#include <memory>

namespace seal
{
class BatchEncoder;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext;

  class BatchEncoder : public api::BatchEncoder
  {
  public:
    explicit BatchEncoder(const EncryptionContext &context);

    BatchEncoder(const BatchEncoder &copy) = default;

    BatchEncoder &operator=(const BatchEncoder &assign) = default;

    BatchEncoder(BatchEncoder &&source) = default;

    BatchEncoder &operator=(BatchEncoder &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    std::size_t slot_count() const override;

    void encode(const std::vector<std::uint64_t> &values_vector, api::Plaintext &destination) const override;

    void encode(const std::vector<std::int64_t> &values_vector, api::Plaintext &destination) const override;

    void decode(const api::Plaintext &plain, std::vector<std::uint64_t> &destination) const override;

    void decode(const api::Plaintext &plain, std::vector<std::int64_t> &destination) const override;

    inline const seal::BatchEncoder &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::BatchEncoder> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
