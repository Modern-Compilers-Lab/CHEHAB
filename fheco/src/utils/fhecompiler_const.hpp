#pragma once

namespace fhecompiler
{

enum Scheme
{
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

enum class sec_level_type
{
  none,
  tc128, // minimum according to he standard
  tc192,
  tc256
};

inline const char *program_not_init_msg =
  "Program was not initialized properly, you need to call init function with all required arguments";

} // namespace fhecompiler
