#pragma once

#include <cstdint>
#include <vector>

namespace api
{
class Encoder
{
  virtual void encode(const std::vector<std::uint64_t> &values_matrix, Plaintext &destination) const = 0;

  virtual void decode(const Plaintext &plain, std::vector<std::uint64_t> &destination) const = 0;
};
} // namespace api
