#include "fheco/fheco.hpp"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

inline fheco::Ciphertext sum_all_slots(fheco::Ciphertext &x, int vector_size)
{
  fheco::Ciphertext result = x;
  int step = 1;

  if (step < 0)
    throw std::logic_error("negative rotation step");

  fheco::Ciphertext rots_sum = x;

  for (; step < vector_size;)
  {
    result += (x << step++);
  }
  // result of sum will be in the first slot
  x <<= vector_size;
  return result;
}

/*
inline fheco::Ciphertext sum_all_slots(fheco::Ciphertext &x, int number_slots_to_rotate)
{
  fheco::Ciphertext result = x;
  int step = number_slots_to_rotate - 1;
  for (; step-- > 0;)
  {
    // fheco::Ciphertext cipher_rotated = rotated_ciphers.back() << 1;
    x <<= 1;
    result += x;
  }
  // result of sum will be in the first slot
  x <<= 1;
  return result;
}
*/

inline fheco::Ciphertext sum_all_slots2(const fheco::Ciphertext &x, size_t vector_size)
{
  fheco::Ciphertext result = x;

  auto clog2 = [](int32_t x) -> int32_t {
    int32_t r = 0;
    while (x > 1)
    {
      x /= 2;
      r += 1;
    }
    return r;
  };

  /*
    A : 8192 x 64
    B : 64 x 256
    n = poly_modulus_degree = 8192/64 = 128

    1 1st 128 lines
    1 2nd 128 lines

    n = 8192

    L is a line of A', L = [1,2,4,5|6,4,3,2|.....] sum of segments

    A'(A packed) : 64 x 64
    B = 64 x 256
    C = A' x B
  */

  auto next_power_of_2 = [&clog2](size_t n) -> size_t {
    if (__builtin_popcount(n) == 1)
      return n;

    auto log2_of_n = clog2(n);

    // I assume no overflow as n will be in range [2^10, 2^16]
    return (1 << (log2_of_n + 1));
  };

  vector_size = next_power_of_2(vector_size);
  int32_t max_num_steps = clog2(vector_size);

  int32_t rot_step = 1;
  for (; max_num_steps--; rot_step *= 2)
  {
    fheco::Ciphertext new_rotated_cipher = x << rot_step;
    result += new_rotated_cipher;
  }
  // result of sum will be in the first slot
  return result;
}

void matrix_mul()
{
  std::vector<std::vector<int64_t>> A; // = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
  std::vector<std::vector<int64_t>> B; // = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

  const int N = 16;
  const int M = 64;
  const int P = 64;
  const int Q = 256;

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

  std::vector<fheco::Ciphertext> A_encrypted;
  // encoding and encryption of A
  size_t vector_size =
    1024; // this the number of slots from user perspective, compiler later may select n > vector_size
  size_t nb_lines_to_pack_in_one_ciphertext = vector_size / A[0].size();
  std::cout << nb_lines_to_pack_in_one_ciphertext << "\n";
  /*
    now nb_lines_to_pack_in_one_ciphertext is the number of lines that needs to be flattened and packed in one
    plaintext to encrypt
  */
  std::cout << "done with encoding and encrypting..\n";
  for (size_t i = 0; i < A.size(); i += nb_lines_to_pack_in_one_ciphertext)
  {
    std::vector<int64_t> lines_flattened;
    for (size_t k = 0; k < nb_lines_to_pack_in_one_ciphertext; k++)
    {
      for (size_t j = 0; j < A[0].size(); j++)
      {
        lines_flattened.push_back(A[i + k][j]);
      }
    }
    // fheco::Ciphertext input("input" + std::to_string(i), fheco::VarType::input);
    fheco::Plaintext pt(lines_flattened);
    A_encrypted.push_back(fheco::encrypt(pt));
    // A_encrypted.push_back(input);
  }
  //  encrypt by column for matrix B
  std::vector<fheco::Plaintext> B_transpose;
  for (size_t column_index = 0; column_index < B[0].size(); column_index++)
  {
    std::vector<int64_t> column_data;
    std::vector<int64_t> column_duplicated;
    for (size_t i = 0; i < B.size(); i++)
    {
      column_data.push_back(B[i][column_index]);
    }
    for (size_t k = 0; k < nb_lines_to_pack_in_one_ciphertext; k++)
    {
      for (auto &v : column_data)
        column_duplicated.push_back(v);
    }
    // fheco::Ciphertext column_encrypted = fheco::Ciphertext::encrypt(column_duplicated);
    B_transpose.push_back(column_duplicated);
  }
  // C contains result of multiplication
  std::vector<fheco::Ciphertext> C_encrypted;
  // make outputs
  std::vector<fheco::Ciphertext> outputs;
  std::vector<std::vector<fheco::Ciphertext>> lines_of_C_encrypted(A.size());

  for (size_t i = 0; i < A_encrypted.size(); i++) // 8
  {
    for (size_t j = 0; j < B_transpose.size(); j++) // 256
    {
      fheco::Ciphertext simd_product = A_encrypted[i] * B_transpose[j];
      /*
        making outputs
        simd_product is now something like [X,X,X,X,X,Y,Y,Y,Y,Y,Z,Z,Z,Z,Z, .....]
        we want to sum up X's with each other and the same thing for Z's and Y's. Same letter means that elements will
        be used to create same entry in result matrix, frequency of each letter is equal to B.size() or A[0].size()
      */
      std::vector<int64_t> mask(vector_size, 0);
      mask[0] = 1;
      for (size_t k = 0; k < nb_lines_to_pack_in_one_ciphertext; k++) // 8192
      {
        size_t number_of_slots_to_sum = A[0].size(); // = B.size()
        fheco::Ciphertext slots_sum = sum_all_slots(simd_product, A[0].size());
        fheco::Ciphertext cipher_with_first_slot_only = slots_sum * mask;

        size_t corresponding_line = i * nb_lines_to_pack_in_one_ciphertext + k;
        lines_of_C_encrypted[corresponding_line].push_back(cipher_with_first_slot_only);
      }
    }
  }

  // making outputs

  for (size_t i = 0; i < lines_of_C_encrypted.size(); i++)
  {

    fheco::Ciphertext output_line; //("output" + std::to_string(i), fheco::VarType::output);
    output_line = lines_of_C_encrypted[i][0];
    for (size_t j = 1; j < lines_of_C_encrypted[i].size(); j++)
    {
      output_line += (lines_of_C_encrypted[i][j] >> j);
    }
    C_encrypted.push_back(output_line);

    for (size_t j = 0; j < lines_of_C_encrypted[i].size(); j++)
    {
      // fheco::Ciphertext output_line("output" + std::to_string(i) + std::to_string(j), fheco::VarType::output);
      // output_line = lines_of_C_encrypted[i][j];
      lines_of_C_encrypted[i][j].set_output("output" + std::to_string(i) + std::to_string(j));
    }
  }
}

int main(int argc, char **argv)
{
  bool call_quantifier = false;
  if (argc > 1)
    call_quantifier = stoi(argv[1]);

  auto ruleset = Compiler::Ruleset::ops_cost;
  if (argc > 2)
    ruleset = static_cast<Compiler::Ruleset>(stoi(argv[2]));

  auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
  if (argc > 3)
    rewrite_heuristic = static_cast<trs::RewriteHeuristic>(stoi(argv[3]));

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  clog << ruleset << "_trs";
  clog << " ";
  clog << rewrite_heuristic;
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
  clog << '\n';

  if (cse)
  {
    Compiler::enable_cse();
    Compiler::enable_order_operands();
  }
  else
  {
    Compiler::disable_cse();
    Compiler::disable_order_operands();
  }

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  t = chrono::high_resolution_clock::now();
  string func_name = "matrix_mul";
  // Compiler::create_func(func_name, 13, 20, false, true);
  const auto &func = Compiler::create_func(func_name, 1024, 27, false, true);
  matrix_mul();

  string gen_name = "_gen_he_" + func_name;
  string gen_path = "he/" + gen_name;
  ofstream header_os(gen_path + ".hpp");
  if (!header_os)
    throw logic_error("failed to create header file");

  ofstream source_os(gen_path + ".cpp");
  if (!source_os)
    throw logic_error("failed to create source file");

  Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
  elapsed = chrono::high_resolution_clock::now() - t;
  cout << elapsed.count() << " ms\n";

  if (call_quantifier)
  {
    util::Quantifier quantifier{func};
    quantifier.run_all_analysis();
    quantifier.print_info(cout);
  }
  return 0;
}
