#include "fheco/trs/term_matcher.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::trs
{
size_t TermMatcher::count_ = 0;

size_t TermMatcher::HashRefWrapp::operator()(const RefWrapp &matcher_ref) const
{
  return hash<TermMatcher>()(matcher_ref.get());
}

bool TermMatcher::EqualRefWrapp::operator()(const RefWrapp &lhs, const RefWrapp &rhs) const
{
  return lhs.get() == rhs.get();
}

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

void TermMatcher::customize_generic_subterms(const unordered_map<size_t, TermMatcherType> &vars_types)
{
  if (is_leaf())
  {
    if (is_variable() && type_ == TermMatcherType::term)
    {
      if (auto it = vars_types.find(id_); it != vars_types.end())
      {
        switch (it->second)
        {
        case TermMatcherType::cipher:
          type_ = TermMatcherType::cipher;
          label_ = "ct_" + *label_;
          break;

        case TermMatcherType::plain:
          type_ = TermMatcherType::plain;
          label_ = "pt_" + *label_;
          break;

        case TermMatcherType::const_:
        case TermMatcherType::term:
          throw invalid_argument("invalid custom term_matcher_type");

        default:
          throw invalid_argument("invalid term_matcher_type");
        }
      }
      else
        throw invalid_argument("custom type for generic variable not provided");
    }
    return;
  }

  for (auto &operand : operands_)
    operand.customize_generic_subterms(vars_types);

  if (type_ == TermMatcherType::term)
    type_ = deduce_result_type(op_code_, operands_);
}

TermMatcher::RefWrappSet TermMatcher::get_variables() const
{
  RefWrappSet result;
  get_variables_util(result);
  return result;
}

void TermMatcher::get_variables_util(RefWrappSet &result) const
{
  if (is_variable())
  {
    result.insert(*this);
    return;
  }
  for (const auto &operand : operands_)
    operand.get_variables_util(result);
}

TermMatcher::RefWrappSet TermMatcher::get_generic_variables() const
{
  RefWrappSet result;
  get_generic_variables_util(result);
  return result;
}

void TermMatcher::get_generic_variables_util(RefWrappSet &result) const
{
  if (is_variable() && type_ == TermMatcherType::term)
  {
    result.insert(*this);
    return;
  }
  for (const auto &operand : operands_)
    operand.get_generic_variables_util(result);
}
} // namespace fheco::trs
