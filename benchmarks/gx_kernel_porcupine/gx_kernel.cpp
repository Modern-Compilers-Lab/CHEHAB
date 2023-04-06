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

void gx_kernel_naive()
{
  Ciphertext c0("c0", 0, 255);
  Ciphertext c1 = c0 << 1;
  Ciphertext c2 = c0 << 5;
  Ciphertext c3 = c0 << 6;
  Ciphertext c4 = c0 << -1;
  Ciphertext c5 = c0 << -4;
  Ciphertext c6 = c0 << -6;
  Ciphertext c7 = c1 + c2;
  Ciphertext c8 = c3 + c4;
  Ciphertext c9 = c5 + c6;
  Ciphertext c10 = c7 + c8;
  Ciphertext c11 = c9 + c9;
  Ciphertext c12 = c10 + c11;
  c12.set_output("c12");
}

void gx_kernel_opt()
{
  Ciphertext c0("c0", 0, 255);
  Ciphertext c1 = c0 << -5;
  Ciphertext c2 = c0 + c1;
  Ciphertext c3 = c2 << 5;
  Ciphertext c4 = c3 + c2;
  Ciphertext c5 = c4 << -1;
  Ciphertext c6 = c4 << 1;
  Ciphertext c7 = c6 - c5;
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

  string func_name = "gx_kernel";
  Compiler::create_func(func_name, vector_size, 16, false, Scheme::bfv);
  gx_kernel_naive();
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

  Compiler::create_func(func_name + "_opt", vector_size, 16, false, Scheme::bfv);
  if (Compiler::evaluate_on_clear(rand_inputs) != outputs)
    cout << "opt version not equivalent to naive\n";

  return 0;
}
