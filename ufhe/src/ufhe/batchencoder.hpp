#pragma once

#include "ufhe/api/ibatchencoder.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/batchencoder.hpp"

namespace ufhe
{
class BatchEncoder : public api::IBatchEncoder
{
public:
  inline BatchEncoder(const EncryptionContext &context)
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

  BatchEncoder(const BatchEncoder &copy) = delete;

  BatchEncoder &operator=(const BatchEncoder &assign) = delete;

  ~BatchEncoder() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t slot_count() override { return underlying().slot_count(); }

  inline void encode(const std::vector<std::uint64_t> &values_vector, api::IPlaintext &destination) const override
  {
    underlying().encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline void encode(const std::vector<std::int64_t> &values_vector, api::IPlaintext &destination) const override
  {
    underlying().encode(values_vector, dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline void decode(const api::IPlaintext &plain, std::vector<std::uint64_t> &destination) const override
  {
    underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
  }

  inline void decode(const api::IPlaintext &plain, std::vector<std::int64_t> &destination) const override
  {
    underlying().decode(dynamic_cast<const Plaintext &>(plain).underlying(), destination);
  }

private:
  inline api::IBatchEncoder &underlying() const { return *underlying_; }

  api::IBatchEncoder *underlying_;
};
} // namespace ufhe
