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

void roberts_cross()
{
  int w = 5;
  Ciphertext c0("c0", 0, 255);
  Ciphertext c0_rot_w = c0 >> w;
  Ciphertext c0_rot_1 = c0 >> 1;
  Ciphertext c1 = c0_rot_w - c0_rot_1;
  Ciphertext c2 = c1 * c1;

  Ciphertext c0_rot_6 = c0 >> (w + 1);
  Ciphertext c3 = c0 - c0_rot_6;
  Ciphertext c4 = c3 * c3;

  Ciphertext c_result = c2 + c4;
  c_result.set_output("c_result");
}

int main(int argc, char **argv)
{
  size_t vector_size = 1024;
  if (argc > 1)
    vector_size = stoi(argv[1]);

  int trs_passes = 1;
  if (argc > 2)
    trs_passes = stoi(argv[2]);

  bool optimize = trs_passes > 0;

  cout << "vector_size: " << vector_size << ", "
       << "trs_passes: " << trs_passes << '\n';

  string func_name = "roberts_cross";
  Compiler::create_func(func_name, vector_size, 20, true, Scheme::bfv);
  roberts_cross();
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
  auto outputs = Compiler::evaluate_on_clear(rand_inputs);
  if (outputs != Compiler::get_example_output_values())
    throw logic_error("compilation correctness-test failed");

  ofstream rand_example_os(func_name + "_rand_example.txt");
  Compiler::print_inputs_outputs(rand_example_os);
  return 0;
}
