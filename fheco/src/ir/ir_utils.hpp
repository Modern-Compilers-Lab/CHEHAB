#pragma once
#include "program.hpp"

namespace ir
{

std::shared_ptr<ir::Term> fold_raw(const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode);
std::shared_ptr<ir::Term> fold_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> multiply_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program);
// needs plain modulus

std::shared_ptr<ir::Term> sum_scalars(
  const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program); // needs plain modulus

std::shared_ptr<ir::Term> subtract_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program);

double get_constant_value_as_double(ir::ConstantValue const_value);

} // namespace ir
