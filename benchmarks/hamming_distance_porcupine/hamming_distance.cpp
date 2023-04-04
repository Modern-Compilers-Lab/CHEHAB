#include "fhecompiler.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void hamming_distance()
{
  Ciphertext c0("c0", 0, 1);
  Ciphertext c1("c1", 0, 1);
  Ciphertext c2 = c1 + c0;
  Ciphertext c2_neg = -c2;
  Ciphertext c3 = c2_neg + 2;
  Ciphertext c4 = c3 * c2;
  Ciphertext c4_rot_2 = c4 << 2;
  Ciphertext c5 = c4_rot_2 + c4;
  Ciphertext c5_rot_1 = c5 << 1;
  Ciphertext c_result = c5_rot_1 + c5;
  c_result.set_output("c_result");
}

int main(int argc, char **argv)
{
  size_t vector_size = 1024;
  if (argc > 1)
    vector_size = stoull(argv[1]);

  int trs_passes = 1;
  if (argc > 2)
    trs_passes = stoi(argv[2]);

  bool optimize = trs_passes > 0;

  cout << "vector_size: " << vector_size << ", "
       << "trs_passes: " << trs_passes << '\n';

  string func_name = "hamming_distance";
  Compiler::create_func(func_name, vector_size, 16, true, Scheme::bfv);
  hamming_distance();
  ofstream init_ir_os(func_name + "_init_ir.dot");
  Compiler::draw_ir(init_ir_os);
  const auto &rand_inputs = Compiler::get_example_input_values();
  ofstream gen_code_os("he/gen_he_" + func_name + ".hpp");
  if (optimize)
    Compiler::compile(gen_code_os, trs_passes);
  else
    Compiler::compile_noopt(gen_code_os);
  ofstream final_ir_os(func_name + "_final_ir.dot");
  Compiler::draw_ir(final_ir_os);
  gen_code_os.close();
  auto outputs = Compiler::evaluate_on_clear(rand_inputs);
  if (outputs != Compiler::get_example_output_values())
    throw logic_error("compilation correctness-test failed");

  ofstream rand_example_os(func_name + "_rand_example.txt");
  Compiler::print_inputs_outputs(rand_example_os);
  return 0;
}
