#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ir
{

using ScalarValue = std::variant<int64_t, double, uint64_t>;

using VectorValue = std::variant<std::vector<int64_t>, std::vector<double>>;

using ConstantValue = std::variant<ScalarValue, VectorValue>;

enum TermType
{
  rawDataType,
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
  rotate_rows,
  rotate_columns,
  square,
  negate,
  exponentiate,
  modswitch,
  relinearize,
  rescale,
  encode,
  decode
};

enum class ConstantTableEntryType
{
  undefined,
  constant,
  input,
  output,
  temp
};

inline std::unordered_map<ir::TermType, std::string> term_type_str = {
  {ir::scalarType, "scalar"}, {ir::plaintextType, "plaintext"}, {ir::ciphertextType, "ciphertext"}};

inline std::unordered_map<ir::OpCode, std::string> str_opcode = {
  {OpCode::undefined, "undefined"},
  {OpCode::add, "add"},
  {OpCode::add_plain, "add_plain"},
  {OpCode::mul, "mul"},
  {OpCode::mul_plain, "mul_plain"},
  {OpCode::sub, "sub"},
  {OpCode::sub_plain, "sub_plain"},
  {OpCode::negate, "negate"},
  {OpCode::modswitch, "modswitch"},
  {OpCode::relinearize, "relinearize"},
  {OpCode::exponentiate, "exponentiate"},
  {OpCode::rescale, "rescale"},
  {OpCode::rotate, "rotate"},
  {OpCode::rotate_columns, "rotate_columns"},
  {OpCode::rotate_rows, "rotate_rows"},
  {OpCode::decode, "decode"},
  {OpCode::encode, "encode"}};

/*
  Encode, Decode
*/

} // namespace ir
