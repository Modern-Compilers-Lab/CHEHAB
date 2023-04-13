#include "op_code.hpp"
#include "term.hpp"
#include <stdexcept>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  int OpCode::count_ = 0;

  const OpCode OpCode::nop = OpCode({}, 0, false, "");
  const OpCode OpCode::encrypt = OpCode({}, 1, false, "encrypt");
  const OpCode OpCode::add = OpCode({}, 2, true, "+");
  const OpCode OpCode::sub = OpCode({}, 2, false, "-");
  const OpCode OpCode::negate = OpCode({}, 1, false, "negate");
  const function<OpCode(int)> OpCode::rotate = [index = count_](int steps) -> OpCode {
    return OpCode(index, {steps}, 1, false, "rotate_" + to_string(steps));
  };
  const OpCode OpCode::square = OpCode({}, 1, false, "square");
  const OpCode OpCode::mul = OpCode({}, 2, true, "mul");
  const OpCode OpCode::mod_switch = OpCode({}, 1, false, "mod_switch");
  const OpCode OpCode::relin = OpCode({}, 1, false, "relin");

  const TermType &OpCode::deduce_result_type(const OpCode &op_code, const vector<Term *> &operands)
  {
    if (op_code.arity_ != operands.size())
      throw invalid_argument("invalid number of operands for op_code");

    switch (op_code.arity_)
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

  bool operator==(const OpCode &lhs, const OpCode &rhs)
  {
    if (lhs.index_ != rhs.index_)
      return false;

    // op_codes with same index (same operation class like rotate_*) must have the same number of generators
    for (size_t i = 0; i < lhs.generators_.size(); ++i)
    {
      if (lhs.generators_[i] != rhs.generators_[i])
        return false;
    }
    return true;
  }

  bool operator<(const OpCode &lhs, const OpCode &rhs)
  {
    if (lhs.index_ == rhs.index_)
    {
      // op_codes with same index (same operation class like rotate_*) must have the same number of generators
      for (size_t i = 0; i < lhs.generators_.size(); ++i)
      {
        if (lhs.generators_[i] < rhs.generators_[i])
          return true;
        else if (lhs.generators_[i] > rhs.generators_[i])
          return false;
      }
      return false;
    }
    return lhs.index_ < rhs.index_;
  }
} // namespace ir
} // namespace fhecompiler
