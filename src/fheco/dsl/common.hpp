#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace fheco
{
using integer = std::int64_t;

using PackedVal = std::vector<integer>;

enum class Scheme
{
  none,
  bfv
};

enum class Backend
{
  none,
  seal
};

enum class SecurityLevel
{
  none,
  tc128,
  tc192,
  tc256
};

void validate_shape(const std::vector<std::size_t> &shape);
} // namespace fheco
