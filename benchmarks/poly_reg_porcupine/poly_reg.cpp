#include "fhecompiler.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void poly_reg()
{
  Ciphertext c0("c0", -10, 10);
  Ciphertext c1("c1", -10, 10);
  Ciphertext c2("c2", -10, 10);
  Ciphertext c3("c3", -10, 10);
  Ciphertext c4("c4", -10, 10);
  Ciphertext c5 = c0 * c4;
  Ciphertext c6 = c5 + c3;
  Ciphertext c7 = c0 * c6;
  Ciphertext c8 = c7 + c2;
  Ciphertext c_result = c1 - c8;
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

  string func_name = "poly_reg";
  Compiler::create_func(func_name, vector_size, 16, true, Scheme::bfv);
  poly_reg();
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
