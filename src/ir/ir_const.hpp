#pragma once

#include <variant>
#include <vector>

namespace ir
{

enum TermType
{
  scalarType,
  ciphertextType,
  plaintextType
};

enum class OpCode
{
  undefined,
  assign,
  add,
  mul,
  sub,
  rotate,
  square,
  negate,
  exponentiate,
  modswitch,
  relinearize,
  rescale
};

enum class ConstantTableEntryType
{
  undefined,
  constant,
  input,
  output,
  temp
};

} // namespace ir
