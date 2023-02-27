#pragma once
#include "program.hpp"
#include <functional>

namespace ir
{

std::shared_ptr<ir::Term> fold_raw(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode);

std::shared_ptr<ir::Term> fold_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> fold_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(double &, double, int64_t)> &e_func, Program *program);

std::shared_ptr<ir::Term> fold_const_plain(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> fold_const_plain_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<double> &, const std::vector<double>, int64_t)> &e_func, Program *program);
// needs plain modulus

std::shared_ptr<ir::Term> fold_plain_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, OpCode opcode, Program *program);

std::shared_ptr<ir::Term> fold_plain_scalar_helper(
  const std::shared_ptr<ir::Term> &_lhs, const std::shared_ptr<ir::Term> &_rhs,
  const std::function<void(std::vector<double> &, double, int64_t)> &e_func, Program *program);

std::shared_ptr<ir::Term> fold_ir_term(const std::shared_ptr<ir::Term> &term, ir::Program *program);

double get_constant_value_as_double(ir::ConstantValue const_value);

void get_constant_value_as_vector_of_double(ir::ConstantValue const_value, std::vector<double> &double_vector);

void cast_int_vector_to_double(const std::vector<int64_t> &int_vector, std::vector<double> &double_vector);

void cast_double_vector_to_int(const std::vector<double> &double_vector, std::vector<int64_t> &int_vector);

ir::TermType deduce_ir_term_type(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

int32_t get_rotation_step(const std::shared_ptr<ir::Term> &node);

int32_t compute_depth_of(const std::shared_ptr<ir::Term> &node);

void print_ops_counters(Program *program);

ConstantTableEntryType deduce_const_table_entry_table(
  const Program::Ptr &lhs, const Program::Ptr &rhs, Program *program);

void swap(Program::Ptr &lhs, Program::Ptr &rhs);

void add_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus);

void multiply_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus);

void subtract_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus);

void add_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus);

void multiply_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus);

void subtract_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus);

void add_scalars(double &lhs, double rhs, int64_t modulus);

void multiply_scalars(double &lhs, double rhs, int64_t modulus);

void subtract_scalars(double &lhs, double rhs, int64_t modulus);
} // namespace ir
