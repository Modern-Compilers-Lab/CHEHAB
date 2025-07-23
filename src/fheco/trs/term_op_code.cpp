#include "fheco/trs/term_matcher.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <stdexcept>
#include <utility>
#include <variant>

using namespace std;
 
namespace fheco::trs
{
const TermOpCode TermOpCode::nop{ir::OpCode::Type::nop, {}, 0, false, "_"};

const TermOpCode TermOpCode::add{ir::OpCode::Type::add, {}, 2, true, "+"};

const TermOpCode TermOpCode::sub{ir::OpCode::Type::sub, {}, 2, false, "-"};

const TermOpCode TermOpCode::negate{ir::OpCode::Type::negate, {}, 1, false, "negate"};

TermOpCode TermOpCode::rotate(OpGenMatcher steps)
{
  return TermOpCode(ir::OpCode::Type::rotate, {move(steps)}, 1, false, "<<");
}
/////////////////////
TermOpCode TermOpCode::SumVec(OpGenMatcher size)
{
  return TermOpCode(ir::OpCode::Type::SumVec, {move(size)}, 1, false, "SumVec");
}
/////////////////////
const TermOpCode TermOpCode::square{ir::OpCode::Type::square, {}, 1, false, "square"};

const TermOpCode TermOpCode::mul{ir::OpCode::Type::mul, {}, 2, true, "*"};

const TermOpCode TermOpCode::relin{ir::OpCode::Type::relin, {}, 1, false, "relin"};

// an alias of o.generators()[0] for the rotate operation
const OpGenMatcher &TermOpCode::steps() const
{
  if (type_ != ir::OpCode::Type::rotate && type_ != ir::OpCode::Type::SumVec )
    throw invalid_argument("steps should be called only on rotate_* operations");

  return generators_[0];
}
} // namespace fheco::trs
