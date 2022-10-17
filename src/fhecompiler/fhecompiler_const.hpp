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

} // namespace fhecompiler
