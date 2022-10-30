#pragma once

#include <cstdint>

namespace ufhe
{
namespace api
{
  enum class BackendType : std::uint8_t
  {
    none = 0x0,
    seal = 0x1
  };
} // namespace api
} // namespace ufhe
