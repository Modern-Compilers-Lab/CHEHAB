#include "fheco/code_gen/common.hpp"
#include "fheco/ir/common.hpp"

using namespace std;

namespace fheco::code_gen
{
size_t HashOpType::operator()(const OpType &op_type) const
{
  size_t h = hash<ir::OpCode::Type>()(op_type.op_code_type_);
  for (auto operand_type : op_type.operands_types_)
    ir::hash_combine(h, operand_type);
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
} // namespace fheco::code_gen
