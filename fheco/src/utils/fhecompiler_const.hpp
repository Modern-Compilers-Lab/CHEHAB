#pragma once

namespace fhecompiler
{

enum Scheme
{
  none,
  bfv,
  bgv,
  ckks
};

enum class VarType
{
  input,
  output,
  constant,
  temp
};

enum class Backend
{
  SEAL,
  HELib,
  OpenFHE
};

enum class SecurityLevel
{
  none,
  tc128, // minimum according to he standard
  tc192,
  tc256
};

} // namespace fhecompiler
