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

double evaluate_raw_op_code(const OpCode &op_code, const vector<Term::Type> &operands_types)
{
  if (Term::deduce_result_type(op_code, operands_types) == Term::Type::plain)
    return 0;

  switch (op_code.type())
  {
  case OpCode::Type::mul:
    if (operands_types[0] == Term::Type::cipher && operands_types[1] == Term::Type::cipher)
      return 1;

    else
      return 0.2;

  case OpCode::Type::square:
    return 0.7;

  case OpCode::Type::encrypt:
    return 0.5;

  case OpCode::Type::relin:
    return 0.25;

  case OpCode::Type::rotate:
    return 0.25;

  case OpCode::Type::mod_switch:
    return 0.05;

  case OpCode::Type::add:
  case OpCode::Type::sub:
  case OpCode::Type::negate:
    return 0.01;

  case OpCode::Type::nop:
    return 0;

  default:
    throw invalid_argument("unhandled op_code raw cost resulting in a ciphertext");
  }
}

double evaluate_raw_op_code(const OpCode &op_code, const vector<Term *> &operands)
{
  vector<Term::Type> operands_types;
  operands_types.reserve(operands.size());
  for (auto operand : operands)
    operands_types.push_back(operand->type());
  return evaluate_raw_op_code(op_code, operands_types);
}

IOValues convert_to_io_values(const IOTermsInfo &io_terms_info)
{
  IOValues io_values;
  for (const auto &[term, io_info] : io_terms_info)
  {
    if (io_info.example_val_)
      io_values.emplace(io_info.label_, *io_info.example_val_);
  }
  return io_values;
}
} // namespace fheco::ir
