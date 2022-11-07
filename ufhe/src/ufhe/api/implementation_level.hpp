#pragma once

#include <cstdint>

namespace ufhe
{
namespace api
{
  enum class implementation_level : std::uint8_t
  {
    // Underlying backend (FHE library) fixed at compile-time
    low_level = 0x1,

    // Underlying backend (FHE library) controlled at runtime
    high_level = 0x2
  };
} // namespace api
} // namespace ufhe
