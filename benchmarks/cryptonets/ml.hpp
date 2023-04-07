#pragma once

#include "fhecompiler.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

std::vector<fhecompiler::Ciphertext> predict(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &x,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Plaintext>>>> &w1,
  const std::vector<fhecompiler::Plaintext> &b1,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Plaintext>>>> &w4,
  const std::vector<fhecompiler::Plaintext> &b4, const std::vector<std::vector<fhecompiler::Plaintext>> &w8,
  const std::vector<fhecompiler::Plaintext> &b8);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> conv_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<std::vector<std::vector<std::vector<fhecompiler::Plaintext>>>> &filters,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> scaled_mean_pool_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input, const std::vector<size_t> &kernel_size,
  const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> pad_2d(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<std::size_t> &kernel_size, const std::vector<std::size_t> &strides);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> add(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input,
  const std::vector<fhecompiler::Plaintext> &b);

std::vector<fhecompiler::Ciphertext> add(
  const std::vector<fhecompiler::Ciphertext> &input, const std::vector<fhecompiler::Plaintext> &b);

std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> square(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input);

std::vector<fhecompiler::Ciphertext> square(const std::vector<fhecompiler::Ciphertext> &input);

std::vector<fhecompiler::Ciphertext> dot(
  const std::vector<fhecompiler::Ciphertext> &input, const std::vector<std::vector<fhecompiler::Plaintext>> &w);

std::vector<fhecompiler::Ciphertext> flatten(
  const std::vector<std::vector<std::vector<fhecompiler::Ciphertext>>> &input);

template <typename T>
inline void print_vector(const std::vector<T> &v, std::ostream &os)
{
  if (v.size() == 0)
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
inline void show_info(
  const std::string &title, const std::chrono::high_resolution_clock::time_point &t, const Tensor &tensor,
  const std::string &var_name)
{
  std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - t;
  std::cout << title << " (" << (elapsed.count()) << " s) " << var_name << " ";
  print_vector(shape(tensor), std::cout);
  std::cout << '\n';
}
