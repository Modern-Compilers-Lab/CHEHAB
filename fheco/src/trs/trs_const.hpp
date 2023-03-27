#pragma once
#include "ir_const.hpp"
#include <unordered_map>
#include <unordered_set>

namespace fheco_trs
{

enum class FunctionId
{
  undefined,
  fold,
  depth,
  count_add,
  count_mul,
  count_rot,
  get_opcode,
  isconst,
  isone,
  iszero,
  type_of
};

enum class TermType
{
  constant,
  variable,
  rawData,
  scalar,
  ciphertext,
  plaintext,
  boolean,
  opcodeAttribute,
  function
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
  less_than_or_equal,
  greater_than_or_equal,
  less_than,
  greater_than,
  not_equal,
  equal,
  _and,
  _or,
  _not
};

// types which can be in matchin condition term

inline std::unordered_set<fheco_trs::TermType> rewrite_condition_types = {
  fheco_trs::TermType::rawData, fheco_trs::TermType::scalar};

inline std::unordered_map<fheco_trs::OpCode, ir::OpCode> opcode_mapping = {
  {fheco_trs::OpCode::undefined, ir::OpCode::undefined},
  {fheco_trs::OpCode::add, ir::OpCode::add},
  {fheco_trs::OpCode::mul, ir::OpCode::mul},
  {fheco_trs::OpCode::sub, ir::OpCode::sub},
  {fheco_trs::OpCode::rotate, ir::OpCode::rotate},
  {fheco_trs::OpCode::rotate_rows, ir::OpCode::rotate_rows},
  {fheco_trs::OpCode::add_plain, ir::OpCode::add_plain},
  {fheco_trs::OpCode::sub_plain, ir::OpCode::sub_plain},
  {fheco_trs::OpCode::mul_plain, ir::OpCode::mul_plain},
  {fheco_trs::OpCode::relinearize, ir::OpCode::relinearize},
  {fheco_trs::OpCode::modswitch, ir::OpCode::modswitch},
  {fheco_trs::OpCode::negate, ir::OpCode::negate},
  {fheco_trs::OpCode::square, ir::OpCode::square},
  {fheco_trs::OpCode::rescale, ir::OpCode::rescale},
  {fheco_trs::OpCode::exponentiate, ir::OpCode::exponentiate}};

inline std::unordered_map<fheco_trs::TermType, std::unordered_set<ir::TermType>> term_type_map = {
  {fheco_trs::TermType::ciphertext, {ir::TermType::ciphertext}},
  {fheco_trs::TermType::plaintext, {ir::TermType::plaintext}},
  {fheco_trs::TermType::scalar, {ir::TermType::scalar}},
  {fheco_trs::TermType::rawData, {ir::TermType::rawData}},
  {fheco_trs::TermType::variable, {ir::TermType::ciphertext, ir::TermType::plaintext}},
  {fheco_trs::TermType::constant, {ir::TermType::plaintext, ir::TermType::scalar, ir::TermType::rawData}}};

} // namespace fheco_trs
