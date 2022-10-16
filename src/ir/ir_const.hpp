#pragma once

#include<vector>
#include<variant>

namespace ir
{

enum TermType
{
  scalarType, ciphertextType, plaintextType
};

enum class OpCode {
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

enum class ConstantTableEntryType { 
  undefined,
  constant, //nor input nor output
  input, 
  output,
  temp//input and output at the same time
};


} //namespace ir