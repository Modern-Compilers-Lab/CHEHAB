#include "fheco/ir/op_code.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/term.hpp"
#include <stdexcept>

using namespace std;

namespace fheco
{
namespace ir
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

    switch (op_code.arity())
    {
    case 0:
      throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");
      break;

    case 1:
      return operands[0]->type();
      break;

    case 2:
      if (operands[0]->type() < operands[1]->type())
        return operands[0]->type();
      else if (operands[0]->type() > operands[1]->type())
        return operands[1]->type();
      else
        return operands[0]->type();
      break;

    default:
      throw invalid_argument("unhandled class of operation with arity");
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
} // namespace ir
} // namespace fheco

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
