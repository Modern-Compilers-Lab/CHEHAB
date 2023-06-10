#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <vector>

namespace fheco::code_gen
{
struct OpType
{
  ir::OpCode::Type op_code_type_;
  std::vector<ir::Term::Type> operands_types_;
};

struct HashOpType
{
  std::size_t operator()(const OpType &op_type) const;
};

struct EqualOpType
{
  bool operator()(const OpType &lhs, const OpType &rhs) const;
};
} // namespace fheco::code_gen
