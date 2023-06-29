#pragma once

#include "fheco/fheco.hpp"
#include <cstddef>
#include <iostream>
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
  const std::vector<std::vector<std::vector<std::vector<fheco::Plaintext>>>> &filters,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> scaled_mean_pool_2d(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<size_t> &kernel_size,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> pad_2d(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<std::size_t> &kernel_size,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> add(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input, const std::vector<fheco::Plaintext> &b);

std::vector<fheco::Ciphertext> add(const std::vector<fheco::Ciphertext> &input, const std::vector<fheco::Plaintext> &b);

std::vector<std::vector<std::vector<fheco::Ciphertext>>> square(
  const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input);

std::vector<fheco::Ciphertext> square(const std::vector<fheco::Ciphertext> &input);

std::vector<fheco::Ciphertext> dot(
  const std::vector<fheco::Ciphertext> &input, const std::vector<std::vector<fheco::Plaintext>> &w);

std::vector<fheco::Ciphertext> flatten(const std::vector<std::vector<std::vector<fheco::Ciphertext>>> &input);

template <typename T>
inline void print_vector(const std::vector<T> &v, std::ostream &os)
{
  if (v.empty())
    return;

  for (std::size_t i = 0; i < v.size() - 1; ++i)
    os << v[i] << " ";
  os << v.back();
}

template <class T>
inline std::vector<std::size_t> shape(const T &v)
{
  return std::vector<size_t>();
}

template <class T>
inline std::vector<size_t> shape(const std::vector<T> &v)
{
  std::vector<std::size_t> sizes = {v.size()};
  auto tmp = shape(v[0]);
  for (auto s : tmp)
    sizes.push_back(s);
  return sizes;
}

template <class Tensor>
inline void show_info(const std::string &title, const Tensor &tensor, const std::string &var_name)
{
  std::cout << title << " " << var_name << " ";
  print_vector(shape(tensor), std::cout);
  std::cout << '\n';
}
