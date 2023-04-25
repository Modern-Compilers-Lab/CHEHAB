#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/function.hpp"
#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <map>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace fheco
{
namespace util
{
  struct OpType
  {
    ir::OpCode::Type op_code_type_;
    std::vector<ir::TermType> operands_types_;
  };

  struct HashOpType
  {
    std::size_t operator()(const OpType &op_type) const;
  };

  struct EqualOpType
  {
    bool operator()(const OpType &lhs, const OpType &rhs) const;
  };

  std::unordered_map<OpType, std::size_t, HashOpType, EqualOpType> count_op_types(ir::Function &func);

  std::map<std::string, std::size_t> group_main_op_types_count(
    const std::unordered_map<OpType, std::size_t, HashOpType, EqualOpType> &op_types_count);
} // namespace util
} // namespace fheco

namespace std
{
std::ostream &operator<<(std::ostream &os, const std::map<std::string, std::size_t> &main_op_types_count);

std::ostream &operator<<(
  std::ostream &os,
  const std::unordered_map<fheco::util::OpType, std::size_t, fheco::util::HashOpType, fheco::util::EqualOpType>
    &op_types_count);
} // namespace std
