#pragma once

#include "ufhe/api/interface.hpp"
#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace ufhe
{
namespace api
{
  class IPlaintext;

  class IBatchEncoder : public Interface
  {
  public:
    virtual std::size_t slot_count() = 0;

    virtual void encode(const std::vector<std::uint64_t> &values_vector, IPlaintext &destination) const = 0;

    virtual void encode(const std::vector<std::int64_t> &values_vector, IPlaintext &destination) const = 0;

    virtual void decode(const IPlaintext &plain, std::vector<std::uint64_t> &destination) const = 0;

    virtual void decode(const IPlaintext &plain, std::vector<std::int64_t> &destination) const = 0;

    // TODO: Handle CKKS
  };
} // namespace api
} // namespace ufhe
