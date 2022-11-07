#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace ufhe
{
namespace api
{
  class Plaintext;

  class BatchEncoder : public AbstractType
  {
  public:
    virtual std::size_t slot_count() const = 0;

    virtual void encode(const std::vector<std::uint64_t> &values_vector, Plaintext &destination) const = 0;

    virtual void encode(const std::vector<std::int64_t> &values_vector, Plaintext &destination) const = 0;

    virtual void decode(const Plaintext &plain, std::vector<std::uint64_t> &destination) const = 0;

    virtual void decode(const Plaintext &plain, std::vector<std::int64_t> &destination) const = 0;

    // TODO: Handle CKKS
  };
} // namespace api
} // namespace ufhe
