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
  encrypt,
  add,
  add_plain,
  mul,
  mul_plain,
  sub,
  sub_plain,
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