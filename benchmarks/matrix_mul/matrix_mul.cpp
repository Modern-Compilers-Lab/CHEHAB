#include "fhecompiler.hpp"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

Ciphertext sum_slots_lin(const Ciphertext &x, int vector_size)
{
  Ciphertext result = x;
  Ciphertext rotated_cipher = x;
  for (int i = 1; i < vector_size; ++i)
  {
    rotated_cipher <<= 1;
    result += rotated_cipher;
  }
  return result;
}

void matrix_mul(int m_a, int n_b, int n_a_m_b, bool use_log_reduction)
{
  // declare inputs
  vector<Ciphertext> A_row_encrypted;
  vector<Ciphertext> B_column_encrypted;
  // encrypt by line for matrix A
  for (int i = 0; i < m_a; ++i)
  {
    Ciphertext line("A[" + to_string(i) + "][]");
    A_row_encrypted.push_back(line);
  }
  // encrypt by column for matrix B
  for (int i = 0; i < n_b; ++i)
  {
    Ciphertext column("B[][" + to_string(i) + "]");
    B_column_encrypted.push_back(column);
  }

  // compute
  vector<Ciphertext> C_row_encrypted;
  for (size_t i = 0; i < A_row_encrypted.size(); ++i)
  {
    Ciphertext cline;
    for (size_t j = 0; j < B_column_encrypted.size(); ++j)
    {
      vector<int64_t> mask(A_row_encrypted.size(), 0);
      mask[j] = 1;
      Ciphertext slot;
      if (use_log_reduction)
        slot = reduce_add(A_row_encrypted[i] * B_column_encrypted[j]);
      else
        slot = sum_slots_lin(A_row_encrypted[i] * B_column_encrypted[j], n_a_m_b);
      if (j == 0)
        cline = slot * mask;
      else
        cline += slot * mask;
    }
    cline.set_output("C[" + to_string(i) + "][]");
    C_row_encrypted.push_back(cline);
  }
}

int main(int argc, char **argv)
{
  int m_a = 16;
  if (argc > 1)
    m_a = stoi(argv[1]);

  int n_a_m_b = 16;
  if (argc > 2)
    n_a_m_b = stoi(argv[2]);

  int n_b = 16;
  if (argc > 3)
    n_b = stoi(argv[3]);

  bool use_log_reduction = true;
  if (argc > 4)
  {
    std::stringstream ss(argv[4]);
    if (!(ss >> boolalpha >> use_log_reduction))
      throw invalid_argument("could not parse use_log_reduction to bools");
  }

  int trs_passes = 1;
  if (argc > 5)
    trs_passes = stoi(argv[5]);

  bool optimize = trs_passes > 0;

  cout << "m_a: " << m_a << ", "
       << "n_a_m_b: " << n_a_m_b << ", "
       << "n_b: " << n_b << ", "
       << "trs_passes: " << trs_passes << "\n";

  string func_name = "matrix_mul";
  Compiler::create_func(func_name, max(m_a, n_b), 16, true, Scheme::bfv);
  matrix_mul(m_a, n_b, n_a_m_b, use_log_reduction);
  Compiler::draw_ir(func_name + "_init_ir.dot");
  const auto &rand_inputs = Compiler::get_input_values();
  if (optimize)
    Compiler::compile("he/gen_he_" + func_name + ".hpp", trs_passes);
  else
    Compiler::compile_noopt("he/gen_he_" + func_name + ".hpp");
  Compiler::draw_ir(func_name + "_final_ir.dot");
  auto outputs = Compiler::evaluate_on_clear(rand_inputs);
  if (outputs != Compiler::get_output_values())
    throw logic_error("compilation correctness-test failed");

  Compiler::serialize_inputs_outputs(func_name + "_rand_example.txt");
  return 0;
}
