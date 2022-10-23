#pragma once

#include <cstdint>

namespace api
{
enum class Backend : std::uint8_t
{
  none = 0x0,

  seal = 0x1
};
} // namespace api
