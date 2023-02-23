#pragma once
#include "ir_const.hpp"
#include "trs_const.hpp"
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fheco_trs
{

class MatchingTerm
{
private:
  fheco_trs::TermType term_type;
  fheco_trs::OpCode opcode = fheco_trs::OpCode::undefined;
  static size_t term_id;
  size_t id;
  std::optional<ir::ConstantValue> value;
  std::optional<std::string> label;
  std::vector<MatchingTerm> operands;
  FunctionId function_id = FunctionId::undefined;

public:
  MatchingTerm() = default;

  MatchingTerm(const MatchingTerm &) = default;
  MatchingTerm &operator=(const MatchingTerm &) = default;
  MatchingTerm(MatchingTerm &&) = default;
  MatchingTerm &operator=(MatchingTerm &&) = default;

  ~MatchingTerm() {}

  MatchingTerm(int64_t, fheco_trs::TermType = TermType::scalarType);
  MatchingTerm(int, fheco_trs::TermType = TermType::scalarType);
  MatchingTerm(double);
  MatchingTerm(fheco_trs::OpCode, const std::vector<MatchingTerm> &, fheco_trs::TermType);
  MatchingTerm(fheco_trs::TermType); // a leaf node
  MatchingTerm(const std::string &, fheco_trs::TermType);
  MatchingTerm(FunctionId func_id);

  void set_value(ir::ConstantValue _value) { value = _value; }

  void set_opcode(fheco_trs::OpCode _opcode) { opcode = _opcode; }

  void set_operands(const std::vector<MatchingTerm> &new_operands) { operands = new_operands; }

  static fheco_trs::TermType deduce_term_type(fheco_trs::TermType, fheco_trs::TermType);

  static MatchingTerm flatten(MatchingTerm term);

  const std::vector<MatchingTerm> &get_operands() const { return operands; }
  size_t get_term_id() const { return id; }
  OpCode get_opcode() const { return opcode; }
  fheco_trs::TermType get_term_type() const { return term_type; }
  std::optional<ir::ConstantValue> get_value() const { return value; }
  const std::optional<std::string> &get_label() const { return label; }
  FunctionId get_function_id() const { return function_id; }
  void set_function_id(FunctionId func_id) { function_id = func_id; }
  void push_operand(const MatchingTerm &operand);

  // utils functions

  static MatchingTerm fold(MatchingTerm m_term);
  static MatchingTerm opcode_of(const MatchingTerm &m_term);
  static MatchingTerm depth_of(const MatchingTerm &m_term);
  static MatchingTerm isconst(const MatchingTerm &m_term);
};

/*
  Operators for matching
*/

MatchingTerm operator*(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator+(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator-(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator-(const MatchingTerm &term);
MatchingTerm operator<<(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm exponentiate(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm square(const MatchingTerm &term);
MatchingTerm relin(const MatchingTerm &term);
MatchingTerm mod_switch(const MatchingTerm &term);

/*
  Operators for rewrite condition
*/

MatchingTerm operator!=(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator&&(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator<(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator<=(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator>(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator>=(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator||(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator==(const MatchingTerm &lhs, const MatchingTerm &rhs);
MatchingTerm operator!(const MatchingTerm &operand);
} // namespace fheco_trs
