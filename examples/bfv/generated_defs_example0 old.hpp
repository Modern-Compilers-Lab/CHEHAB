#include "ufhe/ufhe.hpp"
#include <iomanip>
#include <map>
#include <memory.h>
#include <string>
#include <vector>

// BFV scheme allows modular arithmetic
using Scalar = uint64_t;
using Inputs = std::unordered_map<std::string, std::unique_ptr<ufhe::api::Ciphertext>>;
using Outputs = Inputs;

void example0(
  const ufhe::api::Evaluator &evaluator, const ufhe::api::RelinKeys &relin_keys,
  const ufhe::api::GaloisKeys &galois_keys, Inputs &inputs, Outputs &outputs)
{
  ufhe::api::Ciphertext &a = *inputs["a"];
  ufhe::api::Ciphertext &b = *inputs["b"];
  ufhe::api::Ciphertext &r = *outputs["r"];
  evaluator.add(a, b, r);
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
