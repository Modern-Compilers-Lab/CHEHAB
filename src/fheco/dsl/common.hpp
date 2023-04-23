#pragma once

#include <cstdint>
#include <vector>

namespace fheco
{
using integer = std::int64_t;

using PackedVal = std::vector<integer>;

using ScalarVal = integer;

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
} // namespace fheco
