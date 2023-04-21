#pragma once

#include "program.hpp"

namespace ir
{
std::shared_ptr<ir::Term> fold_raw(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode);
std::shared_ptr<ir::Term> fold_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> fold_const_plain(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> multiply_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program);
// needs plain modulus

std::shared_ptr<ir::Term> sum_scalars(
  const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program); // needs plain modulus

std::shared_ptr<ir::Term> subtract_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program);

std::shared_ptr<ir::Term> sum_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program);

std::shared_ptr<ir::Term> multiply_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program);

std::shared_ptr<ir::Term> subtract_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program);

std::shared_ptr<ir::Term> fold_ir_term(const std::shared_ptr<ir::Term> &term, ir::Program *program);

double get_constant_value_as_double(ir::ConstantValue const_value);

void get_constant_value_as_vector_of_double(ir::ConstantValue const_value, std::vector<double> &double_vector);

void cast_int_vector_to_double(const std::vector<int64_t> &int_vector, std::vector<double> &double_vector);

void cast_double_vector_to_int(const std::vector<double> &double_vector, std::vector<int64_t> &int_vector);

ir::TermType deduce_ir_term_type(const std::vector<ir::Term::Ptr> &operands);

int32_t get_rotation_step(const std::shared_ptr<ir::Term> &node);

int32_t compute_depth_of(const std::shared_ptr<ir::Term> &node);
} // namespace ir
