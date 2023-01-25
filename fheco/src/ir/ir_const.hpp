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

inline std::unordered_map<ir::TermType, std::string> term_type_str = {
  {ir::scalarType, "scalar"}, {ir::plaintextType, "plaintext"}, {ir::ciphertextType, "ciphertext"}};

} // namespace ir
