#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>

using namespace std;

namespace fheco::ir
{
size_t HashPackedVal::operator()(const PackedVal &packed_val) const
{
  size_t h = 0;
  for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
    util::hash_combine(h, *it);
  return h;
}

size_t HashOpType::operator()(const OpType &op_type) const
{
  size_t h = hash<ir::OpCode::Type>()(op_type.op_code_type_);
  for (auto operand_type : op_type.operands_types_)
    util::hash_combine(h, operand_type);
  return h;
}

bool EqualOpType::operator()(const OpType &lhs, const OpType &rhs) const
{
  if (lhs.op_code_type_ != rhs.op_code_type_)
    return false;

  // terms with the same op_code_ must have the same number of operands
  for (size_t i = 0; i < lhs.operands_types_.size(); ++i)
  {
    if (lhs.operands_types_[i] != rhs.operands_types_[i])
      return false;
  }
  return true;
}

IOValues convert_to_io_values(const InputTermsInfo &input_terms_info)
{
  IOValues io_values;
  for (const auto &[term, input_info] : input_terms_info)
  {
    if (input_info.example_val_)
      io_values.emplace(input_info.label_, *input_info.example_val_);
  }
  return io_values;
}

IOValues convert_to_io_values(const OutputTermsInfo &output_terms_info)
{
  IOValues io_values;
  for (const auto &[term, output_info] : output_terms_info)
  {
    if (output_info.example_val_)
    {
      for (const auto &label : output_info.labels_)
        io_values.emplace(label, *output_info.example_val_);
    }
  }
  return io_values;
}

double static_eval_op(const shared_ptr<Func> &func, const OpCode &op_code, const vector<Term *> &operands)
{
  vector<TermInfo> operands_info;
  operands_info.reserve(operands.size());
  for (auto operand : operands)
  {
    auto const_info = func->data_flow().get_const_info(operand);
    if (const_info)
      operands_info.push_back({operand->type(), true, const_info->is_scalar_});
    else
      operands_info.push_back({operand->type(), false, false});
  }
  return static_eval_op(op_code, operands_info);
}

double static_eval_op(const OpCode &op_code, const vector<TermInfo> &operands_info)
{
  if (op_code.type() == OpCode::Type::nop)
  {
    double consts_encoding_cost = 0;
    for (const auto &term_info : operands_info)
    {
      if (term_info.is_const_)
        consts_encoding_cost += 0.01;
    }
    return consts_encoding_cost;
  }

  vector<Term::Type> operands_types;
  operands_types.reserve(operands_info.size());
  transform(operands_info.cbegin(), operands_info.cend(), back_inserter(operands_types), [](const TermInfo &term_info) {
    return term_info.type_;
  });
  if (Term::deduce_result_type(op_code, operands_types) == Term::Type::plain)
    return 0;

  switch (op_code.type())
  {
  case OpCode::Type::mul:
    if (operands_types[0] == operands_types[1])
      return 1;
    else if (operands_info[0].is_scalar_ || operands_info[1].is_scalar_)
      return 0.01;
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

  default:
    throw invalid_argument("unhandled op_code static cost resulting in a ciphertext");
  }
}
} // namespace fheco::ir
