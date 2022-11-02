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

enum class sec_level_type
{
  none,
  tc128, // minimum according to he standard
  tc192,
  tc256
};


} // namespace fhecompiler
