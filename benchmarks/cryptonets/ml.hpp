#pragma once

#include "fhecompiler/fhecompiler.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

using integer = std::int64_t;

std::vector<fhecompiler::Ciphertext> predict(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &x,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Scalar>>>> &w1,
  const std::vector<fhecompiler::Scalar> &b1,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Scalar>>>> &w4,
  const std::vector<fhecompiler::Scalar> &b4, const std::vector<std::vector<fhecompiler::Scalar>> &w8,
  const std::vector<fhecompiler::Scalar> &b8);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> conv_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Scalar>>>> &kernels,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> scaled_mean_pool_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input, const std::vector<size_t> &kernel_shape,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> pad_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<std::size_t> &kernel_shape, const std::vector<std::size_t> &strides);

std::vector<fhecompiler::Ciphertext> add(
  const std::vector<fhecompiler::Ciphertext> &input, const std::vector<fhecompiler::Scalar> &b);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> add(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<fhecompiler::Scalar> &b);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> square(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input);

std::vector<fhecompiler::Ciphertext> square(const std::vector<fhecompiler::Ciphertext> &input);

std::vector<fhecompiler::Ciphertext> dot(
  const std::vector<fhecompiler::Ciphertext> &input, const std::vector<std::vector<fhecompiler::Scalar>> &w);

std::vector<fhecompiler::Ciphertext> flatten(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input);

std::vector<integer> load(std::istream &is);

std::vector<std::vector<integer>> load(std::istream &is, char delim);

std::vector<std::string> split(const std::string &str, char delim);

std::vector<std::vector<std::vector<std::vector<integer>>>> reshape_4d(
  const std::vector<std::vector<integer>> &data, const std::vector<std::size_t> &shape);

template <typename T>
inline void print_vec(const std::vector<T> &v, std::ostream &os)
{
  if (v.empty())
    return;

  for (std::size_t i = 0; i < v.size() - 1; ++i)
    os << v[i] << " ";
  os << v.back();
}

template <typename T>
inline std::vector<std::size_t> shape(const T &v)
{
  return std::vector<size_t>();
}

template <typename T>
inline std::vector<size_t> shape(const std::vector<T> &v)
{
  std::vector<std::size_t> sizes = {v.size()};
  auto tmp = shape(v[0]);
  for (auto s : tmp)
    sizes.push_back(s);
  return sizes;
}

template <typename Tensor>
inline void show_info(const std::string &title, const Tensor &tensor, const std::string &var_name, std::ostream &os)
{
  os << title << " " << var_name << " ";
  print_vec(shape(tensor), os);
  os << '\n';
}
