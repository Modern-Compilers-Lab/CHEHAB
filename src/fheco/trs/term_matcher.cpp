#include "fheco/trs/term_matcher.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::trs
{
size_t TermMatcher::count_ = 0;

TermMatcherType TermMatcher::deduce_result_type(const TermOpCode &op_code, const vector<TermMatcher> &operands)
{
  if (op_code.arity() != operands.size())
    throw invalid_argument("invalid number of operands for op_code");

  if (op_code.arity() == 0)
    throw invalid_argument("cannot deduce result type of operation with 0 operands (nop)");

  // non arithmetic operations
  if (op_code.type() == ir::OpCode::Type::encrypt)
  {
    if (operands[0].type() != TermMatcherType::plain)
      throw invalid_argument("encrypt arg must be plaintext");

    return TermMatcherType::cipher;
  }

  if (op_code.type() == ir::OpCode::Type::mod_switch)
  {
    if (operands[0].type() != TermMatcherType::cipher)
      throw invalid_argument("mod_switch arg must be ciphertext");

    return TermMatcherType::cipher;
  }

  if (op_code.type() == ir::OpCode::Type::relin)
  {
    if (operands[0].type() != TermMatcherType::cipher)
      throw invalid_argument("relin arg must be cipher");

    return TermMatcherType::cipher;
  }

  // arithmetic operations
  switch (op_code.arity())
  {
  case 1:
    return operands[0].type();

  case 2:
    return min(operands[0].type(), operands[1].type());

  default:
    throw logic_error("unhandled term_matcher type deduction for operations with arity > 2");
  }
}
} // namespace fheco::trs
