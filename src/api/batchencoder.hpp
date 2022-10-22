#pragma once

#include "encryptioncontext.hpp"
#include "plaintext.hpp"
#include <cstdint>
#include <vector>

namespace api
{
class BatchEncoder
{
public:
  BatchEncoder(const EncryptionContext &context) { init(context); }

  virtual ~BatchEncoder() {}

  virtual void encode(const std::vector<std::uint64_t> &values_vector, Plaintext &destination) = 0;

  virtual void encode(const std::vector<std::int64_t> &values_vector, Plaintext &destination) = 0;

  virtual void decode(const Plaintext &plain, std::vector<std::uint64_t> &destination) = 0;

  virtual void decode(const Plaintext &plain, std::vector<std::int64_t> &destination) = 0;

  // TODO: Handle CKKS

private:
  virtual void init(const EncryptionContext &context) = 0;
};
} // namespace api
