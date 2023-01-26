#include "matching_term.hpp"
#include "trs_const.hpp"

/*
  x*(y*z) -> (x*y)*z
*/

namespace fheco_trs
{

size_t MatchingTerm::term_id = 0;

MatchingTerm::MatchingTerm(int64_t _value) : term_type(fheco_trs::TermType::scalarType), value(_value), id(term_id++) {}
MatchingTerm::MatchingTerm(int _value) : term_type(fheco_trs::TermType::scalarType), value(_value), id(term_id++) {}
MatchingTerm::MatchingTerm(double _value) : term_type(fheco_trs::TermType::scalarType), value(_value), id(term_id++) {}

MatchingTerm::MatchingTerm(
  fheco_trs::OpCode _opcode, const std::vector<MatchingTerm> &_operands, fheco_trs::TermType _term_type)
  : opcode(_opcode), id(term_id++), operands(_operands), term_type(_term_type)
{}

MatchingTerm::MatchingTerm(fheco_trs::TermType _term_type) : term_type(_term_type), id(term_id++) {}

fheco_trs::TermType MatchingTerm::deduce_term_type(fheco_trs::TermType lhs_term_type, fheco_trs::TermType rhs_term_type)
{

  // Order of if statments is important !!!

  if (lhs_term_type == rhs_term_type)
    return lhs_term_type;

  else if (lhs_term_type == fheco_trs::TermType::ciphertextType || rhs_term_type == fheco_trs::TermType::ciphertextType)
    return fheco_trs::TermType::ciphertextType;

  else if (lhs_term_type == fheco_trs::TermType::plaintextType || rhs_term_type == fheco_trs::TermType::plaintextType)
    return fheco_trs::TermType::plaintextType;

  else
    throw("cannot deduce term type");
}

MatchingTerm MatchingTerm::flatten(MatchingTerm &term)
{
  if (term.get_opcode() == fheco_trs::OpCode::undefined)
    return term;

  else
  {
    std::vector<MatchingTerm> new_operands;
    auto operands = term.get_operands();
    for (auto &operand : operands)
    {
      if (operand.get_opcode() == fheco_trs::OpCode::undefined)
        new_operands.push_back(operand);
      else
      {
        auto sub_operands = operand.get_operands();
        for (auto &sub_operand : sub_operands)
          new_operands.push_back(sub_operand);
      }
    }
    term.set_operands(new_operands);
  }

  return term;
}

MatchingTerm operator*(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::mul, std::vector<MatchingTerm>({lhs, rhs}),
    MatchingTerm::deduce_term_type(lhs.get_term_type(), rhs.get_term_type()));

  if (new_term.get_term_type() == fheco_trs::TermType::ciphertextType && lhs.get_term_type() != rhs.get_term_type())
  {
    new_term.set_opcode(fheco_trs::OpCode::mul_plain);
  }

  return new_term;
}

MatchingTerm operator+(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::add, std::vector<MatchingTerm>({lhs, rhs}),
    MatchingTerm::deduce_term_type(lhs.get_term_type(), rhs.get_term_type()));

  if (new_term.get_term_type() == fheco_trs::TermType::ciphertextType && lhs.get_term_type() != rhs.get_term_type())
  {
    new_term.set_opcode(fheco_trs::OpCode::add_plain);
  }

  return new_term;
}

MatchingTerm operator-(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::sub, std::vector<MatchingTerm>({lhs, rhs}),
    MatchingTerm::deduce_term_type(lhs.get_term_type(), rhs.get_term_type()));

  if (new_term.get_term_type() == fheco_trs::TermType::ciphertextType && lhs.get_term_type() != rhs.get_term_type())
  {
    new_term.set_opcode(fheco_trs::OpCode::sub_plain);
  }

  return new_term;
}

MatchingTerm operator-(const MatchingTerm &term)
{
  MatchingTerm new_term(fheco_trs::OpCode::negate, std::vector<MatchingTerm>({term}), term.get_term_type());
  return new_term;
}

/*
  These are only for ciphertexts
*/
MatchingTerm exponentiate(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (rhs.get_term_type() != fheco_trs::TermType::rawDataType)
    throw("invalid exponentiate expression, exponent must of type rawDataType");

  MatchingTerm new_term(
    fheco_trs::OpCode::exponentiate, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::ciphertextType);
  return new_term;
}

MatchingTerm square(const MatchingTerm &term)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::square, std::vector<MatchingTerm>({term}), fheco_trs::TermType::ciphertextType);
  return new_term;
}

MatchingTerm relin(const MatchingTerm &term)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::relinearize, std::vector<MatchingTerm>({term}), fheco_trs::TermType::ciphertextType);
  return new_term;
}

MatchingTerm mod_switch(const MatchingTerm &term)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::modswitch, std::vector<MatchingTerm>({term}), fheco_trs::TermType::ciphertextType);
  return new_term;
}

MatchingTerm operator<<(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  MatchingTerm new_term(
    fheco_trs::OpCode::rotate_rows, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::ciphertextType);
  return new_term;
}

/*
  Operators for rewrite condition
*/

MatchingTerm operator!=(const MatchingTerm &lhs, const MatchingTerm &rhs)
{

  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::not_equal, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator&&(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::_and, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator<(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::less_than, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator<=(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::less_than_or_equal, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator>(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::greater_than, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator>=(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::greater_than_or_equal, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator||(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::_or, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

MatchingTerm operator==(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  if (
    rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end() ||
    rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
  {
    throw("impossible to evaluate rewrite condition");
  }

  MatchingTerm new_term(
    fheco_trs::OpCode::equal, std::vector<MatchingTerm>({lhs, rhs}), fheco_trs::TermType::booleanType);
  return new_term;
}

/*
  Constant folding
*/

MatchingTerm MatchingTerm::fold(MatchingTerm term_to_fold)
{
  if (term_to_fold.get_term_type() == fheco_trs::TermType::ciphertextType)
    throw("cannot fold ciphertexts");

  term_to_fold.set_fold_flag();
  return term_to_fold;
}

} // namespace fheco_trs
