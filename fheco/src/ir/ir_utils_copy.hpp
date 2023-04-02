#pragma once

#include "program.hpp"
#include <functional>

namespace ir
{

typedef long double Number;
typedef int64_t Modulus;

std::shared_ptr<ir::Term> fold_raw(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode);

std::shared_ptr<ir::Term> fold_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> fold_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(Number &, Number, Modulus)> &e_func, Program *program);

std::shared_ptr<ir::Term> fold_const_plain(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program);

std::shared_ptr<ir::Term> fold_const_plain_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<Number> &, const std::vector<Number>, Modulus)> &e_func, Program *program);
// needs plain modulus

std::shared_ptr<ir::Term> fold_plain_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, OpCode opcode, Program *program);

std::shared_ptr<ir::Term> fold_plain_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<Number> &, Number, Modulus)> &e_func, Program *program);

std::shared_ptr<ir::Term> fold_ir_term(const std::shared_ptr<ir::Term> &term, ir::Program *program);

Number get_constant_value_as_number(ir::ConstantValue const_value);

void get_constant_value_as_vector_of_number(ir::ConstantValue const_value, std::vector<Number> &double_vector);

void cast_int_vector_to_number(const std::vector<int64_t> &int_vector, std::vector<Number> &double_vector);

void cast_number_vector_to_int(const std::vector<Number> &double_vector, std::vector<int64_t> &int_vector);

ir::TermType deduce_ir_term_type(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs);

ir::TermType deduce_ir_term_type(const std::vector<ir::Program::Ptr> &terms);

int32_t get_rotation_step(const std::shared_ptr<ir::Term> &node);

int32_t compute_depth_of(const std::shared_ptr<ir::Term> &node);

void print_ops_counters(Program *program);

ConstantTableEntryType deduce_const_table_entry_table(
  const Program::Ptr &lhs, const Program::Ptr &rhs, Program *program);

void swap(Program::Ptr &lhs, Program::Ptr &rhs);

void add_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus);

void multiply_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus);

void subtract_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus);

void subtract_vector_of_number_to_number(std::vector<Number> &vec, Number v, Modulus modulus);

void add_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus);

void multiply_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus);

void subtract_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus);

void add_numbers(Number &lhs, Number rhs, Modulus modulus);

void multiply_numbers(Number &lhs, Number rhs, Modulus modulus);

void subtract_numbers(Number &lhs, Number rhs, Modulus modulus);

void negate_value_if_possible(const std::string &label, Program *program);

void cast_vector_of_double_to_number(std::vector<Number> &number_vec, const std::vector<double> &double_vec);

void cast_vector_of_number_to_double(std::vector<double> &double_vec, const std::vector<Number> &number_vec);

bool is_a_vector_of_value(const std::vector<Number> &number_vec, const ir::Number &value, size_t vector_size);

bool check_constants_value_equality(const ConstantValue &lhs, const ConstantValue &rhs, ir::TermType term_type);

size_t hash_number(const ir::Number number);

size_t hash_vector_of_numbers(const std::vector<ir::Number> &v);

Program::Ptr get_rotation_node_operand(const Program::Ptr &node);

Program::Ptr get_rotation_step_node(const Program::Ptr &node);

} // namespace ir
