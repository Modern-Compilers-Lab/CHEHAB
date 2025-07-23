#include "fheco/ir/op_code.hpp"
#include "fheco/util/common.hpp"
#include <stdexcept>

using namespace std; 

namespace fheco::ir
{ 
const OpCode OpCode::nop{Type::nop, {}, 0, false, "_"};

const OpCode OpCode::encrypt{Type::encrypt, {}, 1, false, "encrypt"};

const OpCode OpCode::add{Type::add, {}, 2, true, "+"};

const OpCode OpCode::sub{Type::sub, {}, 2, false, "-"};

const OpCode OpCode::negate{Type::negate, {}, 1, false, "negate"};

OpCode OpCode::rotate(int steps)
{
  return OpCode(Type::rotate, {steps}, 1, false, "<< " + to_string(steps));
}
//
OpCode OpCode::SumVec(int size)
{
    return OpCode(Type::SumVec, {size}, 1, false, "SumVec " +to_string(size));
}
///////////

const OpCode OpCode::square{Type::square, {}, 1, false, "square"};

const OpCode OpCode::mul{Type::mul, {}, 2, true, "*"};

const OpCode OpCode::mod_switch{Type::mod_switch, {}, 1, false, "mod_switch"};

const OpCode OpCode::relin{Type::relin, {}, 1, false, "relin"};

// an alias of o.generators()[0] for the rotate operation
int OpCode::steps() const
{
  if (type_ != Type::rotate)
    throw invalid_argument("steps should be called only on rotate_* operations");

  return generators_[0];
}
int OpCode::size() const
{
  if (type_ != Type::SumVec)
    throw invalid_argument("steps should be called only on rotate_* operations");

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

ostream &operator<<(ostream &os, OpCode::Type op_code_type)
{
  switch (op_code_type)
  {
  case OpCode::Type::nop:
    os << "nop";
    break;

  case OpCode::Type::encrypt:
    os << "encrypt";
    break;

  case OpCode::Type::add:
    os << "add";
    break;

  case OpCode::Type::sub:
    os << "sub";
    break;

  case OpCode::Type::negate:
    os << "negate";
    break;

  case OpCode::Type::rotate:
    os << "rotate";
    break;

  case OpCode::Type::square:
    os << "square";
    break;
  //////////////////
  case OpCode::Type::SumVec:
    os << "SumVec";
    break;
  /////////////////
  case OpCode::Type::mul:
    os << "mul";
    break;

  case OpCode::Type::mod_switch:
    os << "mod_switch";
    break;

  case OpCode::Type::relin:
    os << "relin";
    break;

  default:
    throw invalid_argument("invalid op_code type");
  }
  return os;
}
} // namespace fheco::ir

namespace std
{
size_t hash<fheco::ir::OpCode>::operator()(const fheco::ir::OpCode &op_code) const
{
  size_t h = hash<fheco::ir::OpCode::Type>()(op_code.type());
  for (auto g : op_code.generators())
    fheco::util::hash_combine(h, g);
  return h;
}
} // namespace std
