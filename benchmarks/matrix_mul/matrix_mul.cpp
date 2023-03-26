#include "fhecompiler/fhecompiler.hpp"
#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void declare_inputs(int m_a, int n_b, vector<Ciphertext> &A_row_encrypted, vector<Ciphertext> &B_column_encrypted)
{
  // encrypt by line for matrix A
  for (int i = 0; i < m_a; ++i)
  {
    Ciphertext line("A[" + to_string(i) + "][]", VarType::input);
    A_row_encrypted.push_back(line);
  }
  // encrypt by column for matrix B
  for (int i = 0; i < n_b; ++i)
  {
    Ciphertext column("B[][" + to_string(i) + "]", VarType::input);
    B_column_encrypted.push_back(column);
  }
}

void compute_mat_mul(
  const vector<Ciphertext> &A_row_encrypted, const vector<Ciphertext> &B_column_encrypted,
  vector<Ciphertext> &C_row_encrypted, function<void(const Ciphertext &, int, Ciphertext &)> sum_slots_func)
{
  for (size_t i = 0; i < A_row_encrypted.size(); ++i)
  {
    Ciphertext cline;
    for (size_t j = 0; j < B_column_encrypted.size(); ++j)
    {
      vector<int64_t> mask(A_row_encrypted.size(), 0);
      mask[j] = 1;
      Ciphertext slot;
      sum_slots_func(A_row_encrypted[i] * B_column_encrypted[j], A_row_encrypted.size(), slot);
      if (j == 0)
        cline = slot * mask;
      else
        cline += slot * mask;
    }
    C_row_encrypted.push_back(cline);
  }
}

void sum_slots_lin(const Ciphertext &x, int vector_size, Ciphertext &result)
{
  result = x;
  Ciphertext rotated_cipher = x;
  for (int i = 1; i < vector_size; ++i)
  {
    rotated_cipher <<= 1;
    result += rotated_cipher;
  }
}

void sum_slots_log(const Ciphertext &x, int vector_size, Ciphertext &result)
{
  auto clog2 = [](int x) -> int {
    int r = 0;
    while (x > 1)
    {
      x >>= 1;
      r += 1;
    }
    return r;
  };

  result = x;
  int step = vector_size >> 1;
  for (int i = 0; i < clog2(vector_size); ++i)
  {
    result += result << step;
    step >>= 1;
  }
}

void declare_outputs(int m_c, const vector<Ciphertext> &C_row_encrypted)
{
  for (int i = 0; i < m_c; ++i)
  {
    Ciphertext line("C[" + to_string(i) + "][]", VarType::output);
    line = C_row_encrypted[i];
  }
}

int main()
{
  int m_a = 16;
  int n_a = 16;
  int m_b = 16;
  int n_b = 16;
  string dim_info = to_string(m_a) + "_" + to_string(n_a) + "x" + to_string(m_b) + "_" + to_string(n_b);

  string func_name = "matrix_mul_" + dim_info + "_log";
  Compiler::create_func(func_name, max(m_a, n_b));

  vector<Ciphertext> A_row_encrypted;
  vector<Ciphertext> B_column_encrypted;
  declare_inputs(m_a, n_b, A_row_encrypted, B_column_encrypted);
  vector<Ciphertext> C_row_encrypted;
  compute_mat_mul(A_row_encrypted, B_column_encrypted, C_row_encrypted, &sum_slots_log);
  declare_outputs(m_a, C_row_encrypted);

  const utils::variables_values_map &inputs_values = Compiler::get_input_values();

  auto clear_outputs1 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != Compiler::get_output_values())
    throw logic_error("clear_outputs1 != Compiler::get_output_values()");

  Compiler::compile("he/gen_he_" + func_name + ".hpp");

  auto clear_outputs2 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != clear_outputs2)
    throw logic_error("clear_outputs1 != clear_outputs2");

  Compiler::serialize_inputs_outputs(func_name + "_rand_example.txt");

  return 0;
}
