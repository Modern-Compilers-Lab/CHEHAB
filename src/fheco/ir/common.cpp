#include "fheco/ir/common.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::ir
{
size_t HashPackedVal::operator()(const PackedVal &packed_val) const
{
  size_t h = 0;
  for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
    hash_combine(h, *it);
  return h;
}

int64_t evaluate_raw_op_code(const OpCode &op_code, const vector<Term *> &operands)
{
  if (Term::deduce_result_type(op_code, operands) == Term::Type::plain)
    return 0;

  // in decreasing order
  switch (op_code.type())
  {
  case OpCode::Type::mul:
    if (operands[0]->type() == Term::Type::cipher && operands[1]->type() == Term::Type::cipher)
      return 100;

    else
      return 20;

  case OpCode::Type::square:
    return 70;

  case OpCode::Type::encrypt:
    return 50;

  case OpCode::Type::relin:
    return 25;

  case OpCode::Type::rotate:
    return 25;

  case OpCode::Type::mod_switch:
    return 5;

  case OpCode::Type::add:
  case OpCode::Type::sub:
  case OpCode::Type::negate:
    return 1;

  case OpCode::Type::nop:
    return 0;

  default:
    throw invalid_argument("unhandled op_code raw cost resulting in a ciphertext");
  }
}

int64_t evaluate_raw_op_code(const OpCode &op_code, const vector<Term::Type> &operands_types)
{
  if (Term::deduce_result_type(op_code, operands_types) == Term::Type::plain)
    return 0;

  switch (op_code.type())
  {
  case OpCode::Type::mul:
    if (operands_types[0] == Term::Type::cipher && operands_types[1] == Term::Type::cipher)
      return 100;

    else
      return 20;

  case OpCode::Type::square:
    return 70;

  case OpCode::Type::encrypt:
    return 50;

  case OpCode::Type::relin:
    return 25;

  case OpCode::Type::rotate:
    return 25;

  case OpCode::Type::mod_switch:
    return 5;

  case OpCode::Type::add:
  case OpCode::Type::sub:
  case OpCode::Type::negate:
    return 1;

  case OpCode::Type::nop:
    return 0;

  default:
    throw invalid_argument("unhandled op_code raw cost resulting in a ciphertext");
  }
}
} // namespace fheco::ir
