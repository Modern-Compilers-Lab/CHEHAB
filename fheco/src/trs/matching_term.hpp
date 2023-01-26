#pragma once
#include "ir_const.hpp"
#include "trs_const.hpp"
#include <memory>
#include <optional>
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
  std::optional<bool> fold_flag;
  size_t id;
  std::optional<ir::ConstantValue> value;
  std::vector<MatchingTerm> operands;

public:
  MatchingTerm() = default;

  MatchingTerm(const MatchingTerm &) = default;
  MatchingTerm &operator=(const MatchingTerm &) = default;
  MatchingTerm(MatchingTerm &&) = default;
  MatchingTerm &operator=(MatchingTerm &&) = default;

  ~MatchingTerm() {}

  MatchingTerm(int64_t);
  MatchingTerm(int);
  MatchingTerm(double);
  MatchingTerm(fheco_trs::OpCode, const std::vector<MatchingTerm> &, fheco_trs::TermType);
  MatchingTerm(fheco_trs::TermType); // a leaf node

  void set_fold_flag() { fold_flag = true; }

  static MatchingTerm fold(MatchingTerm);

  std::optional<bool> get_fold_flag() const { return fold_flag; }

  void set_opcode(fheco_trs::OpCode _opcode) { opcode = _opcode; }

  void set_operands(const std::vector<MatchingTerm> &new_operands) { operands = new_operands; }

  static fheco_trs::TermType deduce_term_type(fheco_trs::TermType, fheco_trs::TermType);

  static MatchingTerm flatten(MatchingTerm &term);

  const std::vector<MatchingTerm> &get_operands() const { return operands; }
  size_t get_term_id() const { return id; }
  OpCode get_opcode() const { return opcode; }
  fheco_trs::TermType get_term_type() const { return term_type; }
  std::optional<ir::ConstantValue> get_value() const { return value; }
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

} // namespace fheco_trs
