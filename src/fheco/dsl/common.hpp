#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace fheco
{
using integer = std::int64_t;

using PackedVal = std::vector<integer>;

enum class SecurityLevel
{
  none,
  tc128,
  tq128,
  tc192,
  tq192,
  tc256,
  tq256
};

void validate_shape(const std::vector<std::size_t> &shape);
} // namespace fheco
