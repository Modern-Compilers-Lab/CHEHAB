#include "ufhe/ufhe.hpp"
#include <cstdint>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

using ClearInputs = std::unordered_map<std::string, std::vector<int64_t>>;
using EncodedInputs = std::unordered_map<std::string, ufhe::Plaintext>;
using EncryptedInputs = std::unordered_map<std::string, ufhe::Ciphertext>;
using EncodedOutputs = EncodedInputs;
using EncryptedOutputs = EncryptedInputs;

void example0(
  const ufhe::EncryptionContext &context, EncryptedInputs &encrypted_inputs, const ufhe::RelinKeys &relin_keys,
  const ufhe::GaloisKeys &galois_keys, EncryptedOutputs &encrypted_outputs)
{
  ufhe::Evaluator evaluator(context);

  ufhe::Ciphertext &a = encrypted_inputs["a"];
  ufhe::Ciphertext &b = encrypted_inputs["b"];
  ufhe::Ciphertext r;
  evaluator.add(a, b, r);
  encrypted_outputs["r"] = r;
}

/*
Helper function: Prints a matrix of values.
*/
template <typename T>
inline void print_matrix(std::vector<T> matrix, std::size_t row_size)
{
  /*
  We're not going to print every column of the matrix (there are 2048).
  Instead print this many slots from beginning and end of the matrix.
  */
  std::size_t print_size = 5;

  std::cout << std::endl;
  std::cout << "    [";
  for (std::size_t i = 0; i < print_size; i++)
  {
    std::cout << std::setw(3) << std::right << matrix[i] << ",";
  }
  std::cout << std::setw(3) << " ...,";
  for (std::size_t i = row_size - print_size; i < row_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
  }
  std::cout << "    [";
  for (std::size_t i = row_size; i < row_size + print_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ",";
  }
  std::cout << std::setw(3) << " ...,";
  for (std::size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
  }
  std::cout << std::endl;
}
