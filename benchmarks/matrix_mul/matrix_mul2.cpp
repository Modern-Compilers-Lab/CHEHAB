#include "../utils.hpp"
#include "fhecompiler/fhecompiler.hpp"
#include "fhecompiler/normalize_pass.hpp"
#include "fhecompiler/param_selector.hpp"
#include "fhecompiler/quantify_ir.hpp"
#include "fhecompiler/ruleset.hpp"
#include "fhecompiler/trs.hpp"
#include "fhecompiler/trs_util_functions.hpp"
#include <iostream>

inline fhecompiler::Ciphertext sum_all_slots2(const fhecompiler::Ciphertext &x, size_t vector_size)
{
  fhecompiler::Ciphertext result = x;

  auto clog2 = [](int32_t x) -> int32_t {
    int32_t r = 0;
    while (x > 1)
    {
      x /= 2;
      r += 1;
    }
    return r;
  };

  auto next_power_of_2 = [&clog2](size_t n) -> size_t {
    if (__builtin_popcount(n) == 1)
      return n;

    auto log2_of_n = clog2(n);

    // I assume no overflow as n will in range [2^10, 2^16]
    return (1 << (log2_of_n + 1));
  };

  vector_size = next_power_of_2(vector_size);
  int32_t max_num_steps = clog2(vector_size) + 1;

  int32_t rot_step = 1;
  for (; rot_step <= max_num_steps; rot_step *= 2)
  {
    fhecompiler::Ciphertext new_rotated_cipher = result << rot_step;
    result += new_rotated_cipher;
  }
  // result of sum will be in the first slot
  return result;
}

int main()
{

  fhecompiler::init("matrix_mul2", 15);

  std::vector<std::vector<int64_t>> A = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
  std::vector<std::vector<int64_t>> B = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

  /*
  for (size_t i = 0; i < 10; i++)
  {
    fhecompiler::init("matrix_mul2", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    std::vector<std::vector<int64_t>> A = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
    std::vector<std::vector<int64_t>> B = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

    const int N = 50;
    const int M = 50;
    const int P = 50;
    const int Q = 50;
    /*
      for (size_t i = 0; i < N; i++)
      {
        std::vector<int64_t> line;
        for (size_t j = 0; j < M; j++)
        {
          line.push_back((i + 1) * (j + 1));
        }
        A.push_back(line);
      }
      for (size_t i = 0; i < P; i++)
      {
        std::vector<int64_t> line;
        for (size_t j = 0; j < Q; j++)
        {
          line.push_back((i + 1) * (j + 1));
        }
        B.push_back(line);
      }

      */

  std::vector<fhecompiler::Ciphertext> A_encrypted;
  // encrypt by line for matrix A
  for (std::vector<int64_t> line : A)
  {
    fhecompiler::Ciphertext line_encrypted = fhecompiler::Ciphertext::encrypt(line);
    A_encrypted.push_back(line_encrypted);
  }
  // encrypt by column for matrix B
  std::vector<fhecompiler::Ciphertext> B_encrypted;
  for (size_t column_index = 0; column_index < B[0].size(); column_index++)
  {
    std::vector<int64_t> column_data;
    for (size_t i = 0; i < B.size(); i++)
    {
      column_data.push_back(B[i][column_index]);
    }
    fhecompiler::Ciphertext column_encrypted = fhecompiler::Ciphertext::encrypt(column_data);
    B_encrypted.push_back(column_encrypted);
  }

  // C contains result of multiplication
  std::vector<fhecompiler::Ciphertext> C_encrypted;
  // make outputs
  std::vector<fhecompiler::Ciphertext> outputs;

  for (size_t i = 0; i < A.size(); i++)
  {
    std::vector<fhecompiler::Ciphertext> temp_ciphers;
    for (size_t j = 0; j < B[0].size(); j++)
    {
      std::vector<int64_t> mask(A[0].size(), 0);
      mask[0] = 1;
      fhecompiler::Ciphertext simd_product = A_encrypted[i] * B_encrypted[j];
      fhecompiler::Ciphertext temp_cipher = sum_all_slots2(simd_product, A[0].size()) * mask;
      if (j > 0)
        temp_cipher >>= j;
      temp_ciphers.push_back(temp_cipher);
    }
    fhecompiler::Ciphertext c_line = temp_ciphers[0];
    for (size_t k = 1; k < temp_ciphers.size(); k++)
      c_line += temp_ciphers[k];

    fhecompiler::Ciphertext output("output" + std::to_string(i), fhecompiler::VarType::output);
    output = c_line;
  }

  params_selector::EncryptionParameters params;
  benchmarks_utils::set_default_parameters(&params);

  fhecompiler::compile("matrix_mul2.hpp", &params);
}
catch (const char *message)
{
  std::cout << message << "\n";
}

utils::draw_ir(program, "matrix_mul2.hpp1.dot");

auto count = utils::count_main_node_classes(program);
for (const auto &e : count)
  cout << e.first << ":" << e.second << endl;

fheco_passes::CSE cse_pass(program);
// fheco_passes::Normalizer normalizer(program);
// normalizer.normalize();
fheco_trs::TRS trs(program);
trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);

cse_pass.apply_cse2(true);

utils::draw_ir(program, "matrix_mul2.hpp2.dot");

param_selector::ParameterSelector param_selector(program);
param_selector.select_params();

utils::draw_ir(program, "matrix_mul2.hpp3.dot");

cout << endl;

count = utils::count_main_node_classes(program);
for (const auto &e : count)
  cout << e.first << ": " << e.second << endl;

translator::Translator tr(program);
{
  std::ofstream translation_os("matrix_mul2.hpp");

  if (!translation_os)
    throw("couldn't open file for translation.\n");

  tr.translate_program(translation_os);

  translation_os.close();
}

delete program;
program = nullptr;

return 0;
}
