#pragma once

namespace fhecompiler
{
enum class Scheme
{
  none,
  bfv
};

enum class Backend
{
  SEAL
};

enum class SecurityLevel
{
  none,
  tc128, // minimum according to he standard
  tc192,
  tc256
};
} // namespace fhecompiler
