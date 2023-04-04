#pragma once

#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

using encrypted_args_map = std::map<std::string, seal::Ciphertext>;
using encoded_args_map = std::map<std::string, seal::Plaintext>;

struct ClearArgInfo
{
  std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>> value;
  bool is_cipher = false;
  bool is_signed = true;

  inline bool operator==(const ClearArgInfo &e) const
  {
    return (is_cipher == e.is_cipher) && (is_signed == e.is_signed) && (value == e.value);
  }
};

using clear_args_info_map = std::map<std::string, ClearArgInfo>;

void parse_inputs_outputs_file(const std::string &file_name, clear_args_info_map &inputs, clear_args_info_map &outputs);

void prepare_he_inputs(
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const clear_args_info_map &clear_inputs,
  encrypted_args_map &encrypted_inputs, encoded_args_map &encoded_inputs);

void get_clear_outputs(
  const seal::BatchEncoder &encoder, seal::Decryptor &decryptor, const encrypted_args_map &encrypted_outputs,
  const encoded_args_map &encoded_outputs, const clear_args_info_map &ref_clear_outputs,
  clear_args_info_map &clear_outputs);

void print_encrypted_outputs_info(
  const seal::SEALContext &context, seal::Decryptor &decryptor, const encrypted_args_map &encrypted_outputs);

void print_variables_values(const clear_args_info_map &m, std::size_t print_size);

void print_variables_values(const clear_args_info_map &m);

void print_variables_values(const clear_args_info_map &m);

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
