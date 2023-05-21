#include "fheco/ir/term.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::ir
{
size_t Term::count_ = 0;

Term::Term(OpCode op_code, vector<Term *> operands)
  : id_{++count_}, op_code_{move(op_code)}, operands_{move(operands)}, type_{deduce_result_type(op_code_, operands)}
{}

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

string Term::term_type_str_repr(Type term_type)
{
  switch (term_type)
  {
  case Type::cipher:
    return "ctxt";

  case Type::plain:
    return "ptxt";

  default:
    throw invalid_argument("invalid term_type");
  }
}

Term::Type Term::deduce_result_type(const OpCode &op_code, const vector<Term *> &operands)
{
  if (op_code.arity() != operands.size())
    throw invalid_argument("invalid number of operands for op_code");

  if (op_code.arity() == 0)
    throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");

  // non arithmetic operations
  if (op_code.type() == OpCode::Type::encrypt)
  {
    if (operands[0]->type() != Type::plain)
      throw invalid_argument("encrypt arg must be plaintext");

    return Type::cipher;
  }

  if (op_code.type() == OpCode::Type::mod_switch)
  {
    if (operands[0]->type() != Type::cipher)
      throw invalid_argument("mod_switch arg must be ciphertext");

    return Type::cipher;
  }

  if (op_code.type() == OpCode::Type::relin)
  {
    if (operands[0]->type() != Type::cipher)
      throw invalid_argument("relin arg must be cipher");

    return Type::cipher;
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

Term::Type Term::deduce_result_type(const OpCode &op_code, const vector<Type> &operands_types)
{
  if (op_code.arity() != operands_types.size())
    throw invalid_argument("invalid number of operands for op_code");

  if (op_code.arity() == 0)
    throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");

  // non arithmetic operations
  if (op_code.type() == OpCode::Type::encrypt)
  {
    if (operands_types[0] != Type::plain)
      throw invalid_argument("encrypt arg must be plaintext");

    return Type::cipher;
  }

  if (op_code.type() == OpCode::Type::mod_switch)
  {
    if (operands_types[0] != Type::cipher)
      throw invalid_argument("mod_switch arg must be ciphertext");

    return Type::cipher;
  }

  if (op_code.type() == OpCode::Type::relin)
  {
    if (operands_types[0] != Type::cipher)
      throw invalid_argument("relin arg must be cipher");

    return Type::cipher;
  }

  // arithmetic operations
  switch (op_code.arity())
  {
  case 1:
    return operands_types[0];

  case 2:
    return min(operands_types[0], operands_types[1]);

  default:
    throw logic_error("unhandled term type deduction for operations with arity > 2");
  }
}
} // namespace fheco::ir
