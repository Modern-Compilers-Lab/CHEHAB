#include "fheco/ir/op_code.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/term.hpp"
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace fheco::ir

{
const OpCode OpCode::nop = OpCode(OpCode::Type::nop, {}, 0, false, "_");
const OpCode OpCode::encrypt = OpCode(OpCode::Type::encrypt, {}, 1, false, "encrypt");
const OpCode OpCode::add = OpCode(OpCode::Type::add, {}, 2, true, "+");
const OpCode OpCode::sub = OpCode(OpCode::Type::sub, {}, 2, false, "-");
const OpCode OpCode::negate = OpCode(OpCode::Type::negate, {}, 1, false, "negate");
OpCode OpCode::rotate(int steps)
{
  return OpCode(OpCode::Type::rotate, {steps}, 1, false, "rotate_" + to_string(steps));
}
const OpCode OpCode::square = OpCode(OpCode::Type::square, {}, 1, false, "square");
const OpCode OpCode::mul = OpCode(OpCode::Type::mul, {}, 2, true, "*");
const OpCode OpCode::mod_switch = OpCode(OpCode::Type::mod_switch, {}, 1, false, "mod_switch");
const OpCode OpCode::relin = OpCode(OpCode::Type::relin, {}, 1, false, "relin");

TermType OpCode::deduce_result_type(const OpCode &op_code, const vector<Term *> &operands)
{
  if (op_code.arity() != operands.size())
    throw invalid_argument("invalid number of operands for op_code");

  if (op_code.arity() == 0)
    throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");

  // non arithmetic operations
  if (op_code.type() == OpCode::Type::encrypt)
  {
    if (operands[0]->type() != TermType::plaintext)
      throw invalid_argument("encrypt arg must be plaintext");

    return TermType::ciphertext;
  }

  if (op_code.type() == OpCode::Type::mod_switch)
  {
    if (operands[0]->type() != TermType::ciphertext)
      throw invalid_argument("mod_switch arg must be ciphertext");

    return TermType::ciphertext;
  }

  if (op_code.type() == OpCode::Type::relin)
  {
    if (operands[0]->type() != TermType::ciphertext)
      throw invalid_argument("relin arg must be cipher");

    return TermType::ciphertext;
  }

  // arithmetic operations

  auto term_with_min_type_it =
    min_element(operands.begin(), operands.end(), [](const Term *lhs, const Term *rhs) -> bool {
      return lhs->type() < rhs->type();
    });
  return (*term_with_min_type_it)->type();
}

OpCode OpCode::sample_op_code_from_type(OpCode::Type type)
{
  switch (type)
  {
  case OpCode::Type::nop:
    return OpCode::nop;
    break;

  case OpCode::Type::encrypt:
    return OpCode::encrypt;
    break;

  case OpCode::Type::add:
    return OpCode::add;
    break;

  case OpCode::Type::sub:
    return OpCode::sub;
    break;

  case OpCode::Type::negate:
    return OpCode::negate;
    break;

  case OpCode::Type::rotate:
    return OpCode::rotate(0);
    break;

  case OpCode::Type::square:
    return OpCode::square;
    break;

  case OpCode::Type::mul:
    return OpCode::mul;
    break;

  case OpCode::Type::mod_switch:
    return OpCode::mod_switch;
    break;

  case OpCode::Type::relin:
    return OpCode::relin;
    break;

  default:
    throw invalid_argument("invalid op_code type");
    break;
  }
}

// an alias of o.generators()[0] for the rotate operation
int OpCode::steps() const
{
  if (type_ != Type::rotate)
    throw std::invalid_argument("steps should be called only on rotate_* operations");

  return generators_[0];
}

bool operator==(const OpCode &lhs, const OpCode &rhs)
{
  if (lhs.type() != rhs.type())
    return false;

  // op_codes with same id (same operation type like rotate_*) must have the same number of generators
  for (size_t i = 0; i < lhs.generators().size(); ++i)
  {
    if (lhs.generators()[i] != rhs.generators()[i])
      return false;
  }
  return true;
}

bool operator<(const OpCode &lhs, const OpCode &rhs)
{
  if (lhs.type() == rhs.type())
  {
    // op_codes with same id (same operation type like rotate_*) must have the same number of generators
    for (size_t i = 0; i < lhs.generators().size(); ++i)
    {
      if (lhs.generators()[i] < rhs.generators()[i])
        return true;
      else if (lhs.generators()[i] > rhs.generators()[i])
        return false;
    }
    return false;
  }
  return lhs.type() < rhs.type();
}
} // namespace fheco::ir

namespace std
{
size_t hash<fheco::ir::OpCode>::operator()(const fheco::ir::OpCode &op_code) const
{
  size_t h = hash<fheco::ir::OpCode::Type>()(op_code.type());
  for (auto g : op_code.generators())
    fheco::ir::hash_combine(h, g);
  return h;
}
} // namespace std
