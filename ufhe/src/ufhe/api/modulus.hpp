#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace ufhe
{
namespace api
{
  class Modulus : public AbstractType
  {
  public:
    using vector = std::vector<std::reference_wrapper<const Modulus>>;

    virtual int bit_count() const = 0;

    virtual std::uint64_t value() const = 0;

    virtual bool is_prime() const = 0;

    virtual bool operator==(const Modulus &compare) const = 0;

    virtual bool operator!=(const Modulus &compare) const = 0;

    virtual bool operator<(const Modulus &compare) const = 0;

    virtual bool operator<=(const Modulus &compare) const = 0;

    virtual bool operator>(const Modulus &compare) const = 0;

    virtual bool operator>=(const Modulus &compare) const = 0;

    virtual std::uint64_t reduce(std::uint64_t value) const = 0;

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
