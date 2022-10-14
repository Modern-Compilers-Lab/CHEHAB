#pragma once

#include<vector>
#include<variant>

namespace ir
{

enum TermType
{
  scalarType, ciphertextType, plaintextType
};

enum OpCode {
  undefined,
  constant,
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

} //namespace ir