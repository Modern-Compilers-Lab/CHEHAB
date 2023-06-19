#include "fheco/trs/op_gen_op_code.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::trs
{
const OpGenOpCode OpGenOpCode::nop{Type::nop, 0, "_"};

const OpGenOpCode OpGenOpCode::add{Type::add, 2, "+"};

const OpGenOpCode OpGenOpCode::sub{Type::sub, 2, "-"};

const OpGenOpCode OpGenOpCode::negate{Type::negate, 1, "negate"};

const OpGenOpCode OpGenOpCode::mod{Type::mod, 2, "%"};

ostream &operator<<(ostream &os, OpGenOpCode::Type op_code_type)
{
  switch (op_code_type)
  {
  case OpGenOpCode::Type::nop:
    os << "nop";
    break;

  case OpGenOpCode::Type::add:
    os << "add";
    break;

  case OpGenOpCode::Type::sub:
    os << "sub";
    break;

  case OpGenOpCode::Type::negate:
    os << "negate";
    break;

  case OpGenOpCode::Type::mod:
    os << "mod";
    break;

  default:
    throw invalid_argument("invalid op_gen_op_code type");
  }
  return os;
}
} // namespace fheco::trs
