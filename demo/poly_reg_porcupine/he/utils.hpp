#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "seal/seal.h"

using EncryptedArgs = std::unordered_map<std::string, seal::Ciphertext>;
using EncodedArgs = std::unordered_map<std::string, seal::Plaintext>;

struct ClearArgInfo
{
  std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>> value_;
  bool is_cipher_;
  bool is_signed_;
};

inline bool operator==(const ClearArgInfo &lhs, const ClearArgInfo &rhs)
{
  return lhs.value_ == rhs.value_;
}

using ClearArgsInfo = std::unordered_map<std::string, ClearArgInfo>;

void parse_inputs_outputs_file(
  const seal::Modulus &plain_modulus, std::istream &is, ClearArgsInfo &inputs, ClearArgsInfo &outputs);

void prepare_he_inputs(
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const ClearArgsInfo &clear_inputs,
  EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs);

void get_clear_outputs(
  const seal::BatchEncoder &encoder, seal::Decryptor &decryptor, const EncryptedArgs &encrypted_outputs,
  const EncodedArgs &encoded_outputs, const ClearArgsInfo &ref_clear_outputs, ClearArgsInfo &clear_outputs);

void print_encrypted_outputs_info(
  const seal::SEALContext &context, seal::Decryptor &decryptor, const EncryptedArgs &encrypted_outputs,
  std::ostream &os);

void print_variables_values(const ClearArgsInfo &m, std::size_t print_size, std::ostream &os);

void print_variables_values(const ClearArgsInfo &m, std::ostream &os);

template <typename T>
inline void print_vector(const std::vector<T> &v, std::ostream &os, std::size_t print_size)
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
inline void print_vector(const std::vector<T> &v, std::ostream &os)
{
  if (v.size() == 0)
    return;

  for (std::size_t i = 0; i < v.size() - 1; ++i)
    os << v[i] << " ";
  os << v.back();
}
