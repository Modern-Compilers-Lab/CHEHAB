#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "seal/seal.h"

using Number = __int128;

using EncryptedArgs = std::unordered_map<std::string, seal::Ciphertext>;
using EncodedArgs = std::unordered_map<std::string, seal::Plaintext>;

template <typename T>
inline std::vector<T> load(std::istream &is)
{
  std::vector<T> data;
  std::string line;
  while (std::getline(is, line))
    data.push_back(static_cast<T>(std::stod(line)));
  return data;
}

std::vector<std::vector<double>> load(std::istream &is, char delim);

std::vector<std::string> split(const std::string &str, char delim);

std::vector<std::vector<std::vector<std::vector<double>>>> reshape_4d(
  const std::vector<std::vector<double>> &data, const std::vector<std::size_t> &shape);

template <typename T>
std::vector<T> subset(const std::vector<T> &vec, std::size_t begin, std::size_t end)
{
  end = vec.size() < end ? vec.size() : end;
  std::vector<T> subset_vec;
  subset_vec.reserve(end - begin);
  for (auto i = begin; i < end; ++i)
    subset_vec.push_back(vec[i]);
  return subset_vec;
}

Number scale(double n, const Number &scaler);

std::vector<Number> scale(const std::vector<double> &data, const Number &scaler);

std::vector<std::vector<Number>> scale(const std::vector<std::vector<double>> &data, const Number &scaler);

std::vector<std::vector<std::vector<Number>>> scale(
  const std::vector<std::vector<std::vector<double>>> &data, const Number &scaler);

std::vector<std::vector<std::vector<std::vector<Number>>>> scale(
  const std::vector<std::vector<std::vector<std::vector<double>>>> &data, const Number &scaler);

// needed for big shifts
template <typename T>
inline T shift(T n, int step)
{
  while (step--)
    n += n;
  return n;
}

void export_reduced_weights_biases(
  const Number &modulus, std::uint64_t plain_modulus, int w1_precis, int w4_precis, int w8_precis, int b1_precis,
  int b4_precis, int b8_precis);

std::vector<std::uint64_t> reduce(const std::vector<Number> &data, const Number &modulus, std::uint64_t plain_modulus);

std::vector<std::vector<std::uint64_t>> reduce(
  const std::vector<std::vector<Number>> &data, const Number &modulus, std::uint64_t plain_modulus);

void print_reduced_data(const std::vector<std::uint64_t> &data, std::ostream &os);

void print_reduced_data(const std::vector<std::vector<std::uint64_t>> &data, std::ostream &os, char delim);

std::vector<std::vector<Number>> reshape_order(
  const std::vector<std::vector<Number>> &data, const std::vector<std::size_t> &order);

std::vector<std::vector<std::vector<Number>>> reshape_order(
  const std::vector<std::vector<std::vector<Number>>> &data, const std::vector<std::size_t> &order);

std::vector<std::vector<std::vector<std::vector<Number>>>> reshape_order(
  const std::vector<std::vector<std::vector<std::vector<Number>>>> &data, const std::vector<std::size_t> &order);

void prepare_he_inputs(
  const Number &modulus, std::uint64_t plain_modulus, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const std::vector<std::vector<std::vector<std::vector<Number>>>> &x,
  EncryptedArgs &encrypted_inputs);

std::vector<std::vector<Number>> get_clear_outputs(
  const std::vector<std::uint64_t> &coprimes, const Number &modulus,
  const std::vector<std::unique_ptr<seal::BatchEncoder>> &encoders,
  std::vector<std::unique_ptr<seal::Decryptor>> &decryptors,
  const std::vector<EncryptedArgs> &primes_encrypted_outputs);

std::vector<std::size_t> argmax(const std::vector<std::vector<Number>> &data);

std::size_t count_equal(const std::vector<std::uint64_t> &a, const std::vector<std::uint64_t> &b);

void print_encrypted_outputs_info(
  const seal::SEALContext &context, seal::Decryptor &decryptor, const EncryptedArgs &encrypted_outputs,
  std::ostream &os);

template <typename T>
inline std::vector<std::size_t> shape(const T &val)
{
  return std::vector<std::size_t>();
}

template <typename T>
inline std::vector<std::size_t> shape(const std::vector<T> &vec)
{
  std::vector<std::size_t> sizes = {vec.size()};
  auto tmp = shape(vec[0]);
  for (auto s : tmp)
    sizes.push_back(s);
  return sizes;
}

template <typename T>
inline void print_vec(const std::vector<T> &v, std::ostream &os, std::size_t print_size)
{
  std::size_t size = v.size();
  if (size < 2 * print_size)
    throw std::invalid_argument("vector size must at least twice print_size");

  if (size == 0)
    return;

  for (std::size_t i = 0; i < print_size; ++i)
    os << v[i] << " ";
  if (v.size() > 2 * print_size)
    os << "... ";
  for (std::size_t i = size - print_size; i < size - 1; ++i)
    os << v[i] << " ";
  os << v.back();
}

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
