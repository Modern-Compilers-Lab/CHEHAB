#pragma once

#include "fheco/fheco.hpp"
#include <cstddef>
#include <istream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

std::vector<fheco::Ciphertext> predict(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &x,
  const std::vector<std::vector<std::vector<std::vector<fheco::Plaintext>>>> &w1,
  const std::vector<fheco::Plaintext> &b1,
  const std::vector<std::vector<std::vector<std::vector<fheco::Plaintext>>>> &w4,
  const std::vector<fheco::Plaintext> &b4, const std::vector<std::vector<fheco::Plaintext>> &w8,
  const std::vector<fheco::Plaintext> &b8);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> conv_2d(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input,
  const std::vector<std::vector<std::vector<std::vector<fheco::Plaintext>>>> &kernels,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> scaled_mean_pool_2d(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<size_t> &kernel_shape,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> pad_2d(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<std::size_t> &kernel_shape,
  const std::vector<std::size_t> &strides);

std::vector<fheco::Ciphertext> add(const std::vector<fheco::Ciphertext> &input, const std::vector<fheco::Plaintext> &b);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> add(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<fheco::Plaintext> &b);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> square(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input);

std::vector<fheco::Ciphertext> square(const std::vector<fheco::Ciphertext> &input);

std::vector<fheco::Ciphertext> dot(
  const std::vector<fheco::Ciphertext> &input, const std::vector<std::vector<fheco::Plaintext>> &w);

std::vector<fheco::Ciphertext> flatten(const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input);

std::vector<fheco::integer> load(std::istream &is);

std::vector<std::vector<fheco::integer>> load(std::istream &is, char delim);

std::vector<std::string> split(const std::string &str, char delim);

std::vector<std::vector<std::vector<std::vector<fheco::integer>>>> reshape_4d(
  const std::vector<std::vector<fheco::integer>> &data, const std::vector<std::size_t> &shape);

template <typename T>
inline void print_vec(const std::vector<T> &v, std::ostream &os)
{
  if (v.empty())
    return;

  for (auto it = v.cbegin();;)
  {
    os << *it;
    ++it;
    if (it == v.cend())
      break;

    os << " ";
  }
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
