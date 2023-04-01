#pragma once

#include "clear_data_evaluator.hpp"
#include "ir_const.hpp"
#include "program.hpp"
#include <cstdint>
#include <map>
#include <ostream>
#include <random>
#include <string>
#include <vector>

namespace utils
{
using variables_values_map = std::map<std::string, ir::ConstantValue>;

using io_variables_values = std::map<std::string, ir::VectorValue>;

io_variables_values evaluate_on_clear(ir::Program *program, const io_variables_values &inputs_values);

ir::VectorValue init_const(const ClearDataEvaluator &evaluator, const ir::VectorValue &value_var);

ir::ScalarValue init_const(const ClearDataEvaluator &evaluator, const ir::ScalarValue &value_var);

ir::VectorValue operate_unary(const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg);

ir::ScalarValue operate_unary(const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::ScalarValue &arg);

ir::VectorValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg1, const ir::VectorValue &arg2);

ir::VectorValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg1, const ir::ScalarValue &arg2);

inline ir::VectorValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::ScalarValue &arg1, const ir::VectorValue &arg2)
{
  return operate_binary(evaluator, op, arg2, arg1);
}

ir::ScalarValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::ScalarValue &arg1, const ir::ScalarValue &arg2);

ir::VectorValue operate_rotate(const ClearDataEvaluator &evaluator, const ir::VectorValue &arg, int steps);

template <typename T>
inline void init_random(std::vector<T> &v, long long min_value, long long max_value)
{
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<long long> uni(min_value, max_value);
  for (std::size_t i = 0; i < v.size(); ++i)
    v[i] = static_cast<T>(uni(rng));
}

void print_variables_values(const io_variables_values &m, std::size_t lead_trail_size, std::ostream &os);

template <typename T>
inline void print_vector(const std::vector<T> &v, std::size_t lead_trail_size, std::ostream &os)
{
  std::size_t size = v.size();
  if (size < 2 * lead_trail_size)
    throw std::invalid_argument("vector size must at least twice print_size");

  if (size == 0)
    return;

  for (std::size_t i = 0; i < lead_trail_size; ++i)
    os << v[i] << " ";
  if (v.size() > 2 * lead_trail_size)
    os << "... ";
  for (std::size_t i = size - lead_trail_size; i < size - 1; ++i)
    os << v[i] << " ";
  os << v.back();
}
} // namespace utils

namespace std
{
template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
  if (v.size() == 0)
    return os;

  for (std::size_t i = 0; i < v.size() - 1; ++i)
    os << v[i] << " ";
  os << v.back();
  return os;
}

std::ostream &operator<<(std::ostream &os, const utils::io_variables_values &m);
} // namespace std
