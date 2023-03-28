#pragma once

#include <climits>
#include <cstddef>

namespace utils
{
inline int ilog2(std::size_t x)
{
  return sizeof(std::size_t) * CHAR_BIT - __builtin_clz(x) - 1;
}

} // namespace utils
