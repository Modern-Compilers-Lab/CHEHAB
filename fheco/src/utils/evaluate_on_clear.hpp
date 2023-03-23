#pragma once

#include "program.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <ostream>
#include <random>
#include <string>
#include <variant>
#include <vector>

namespace utils
{
using variables_values_map = std::map<std::string, std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>>>;

template <class>
inline constexpr bool always_false_v = false;

variables_values_map evaluate_on_clear(ir::Program *program, const variables_values_map &inputs_values);

template <typename T>
inline void init_random(std::vector<T> &v, long long min_value = 0, long long max_value = 100)
{
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<long long> uni(min_value, max_value);
  for (std::size_t i = 0; i < v.size(); ++i)
    v[i] = static_cast<T>(uni(rng));
}

void print_variables_values(const variables_values_map &m, std::size_t print_size = 4);

template <typename T>
inline void print_vector(const std::vector<T> &v, std::size_t print_size)
{
  if (v.size() < 2 * print_size)
    throw std::invalid_argument("vector size must at least twice print_size");

  std::size_t size = v.size();
  std::cout << "[";
  for (std::size_t i = 0; i < print_size; i++)
    std::cout << v[i] << ", ";
  if (v.size() > 2 * print_size)
    std::cout << " ..., ";
  for (std::size_t i = size - print_size; i < size; i++)
    std::cout << v[i] << ((i != size - 1) ? ", " : "]\n");
}

template <typename T>
inline void serialize_vector(const std::vector<T> &v, std::ostream &os)
{
  for (auto it = v.begin(); it != v.end() - 1; ++it)
    os << *it << " ";
  os << *v.rbegin();
}

} // namespace utils
