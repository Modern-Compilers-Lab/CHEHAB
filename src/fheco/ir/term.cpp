#include "fheco/ir/term.hpp"
#include "fheco/ir/common.hpp"

using namespace std;

namespace fheco::ir
{
size_t Term::count_ = 0;

size_t Term::HashPtr::operator()(const Term *p) const
{
  return hash<Term>()(*p);
}

bool Term::EqualPtr::operator()(const Term *lhs, const Term *rhs) const
{
  return *lhs == *rhs;
}

bool Term::ComparePtr::operator()(const Term *lhs, const Term *rhs) const
{
  return *lhs < *rhs;
}

TermType Term::deduce_result_type(const OpCode &op_code, const vector<Term *> &operands)
{
  if (op_code.arity() != operands.size())
    throw invalid_argument("invalid number of operands for op_code");

  if (op_code.arity() == 0)
    throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");

  // non arithmetic operations
  if (op_code.type() == OpCode::Type::encrypt)
  {
    if (operands[0]->type() != TermType::plain)
      throw invalid_argument("encrypt arg must be plaintext");

    return TermType::cipher;
  }

  if (op_code.type() == OpCode::Type::mod_switch)
  {
    if (operands[0]->type() != TermType::cipher)
      throw invalid_argument("mod_switch arg must be ciphertext");

    return TermType::cipher;
  }

  if (op_code.type() == OpCode::Type::relin)
  {
    if (operands[0]->type() != TermType::cipher)
      throw invalid_argument("relin arg must be cipher");

    return TermType::cipher;
  }

  // arithmetic operations
  switch (op_code.arity())
  {
  case 1:
    return operands[0]->type();

  case 2:
    return min(operands[0]->type(), operands[1]->type());

  default:
    throw logic_error("unhandled term type deduction for operations with arity > 2");
  }
}
} // namespace fheco::ir
