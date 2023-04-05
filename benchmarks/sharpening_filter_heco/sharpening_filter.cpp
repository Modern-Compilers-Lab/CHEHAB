#include "fhecompiler.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void sharpening_filter_opt(int width, int height)
{
  int n = width;
  Ciphertext img("img", 0, 255);
  Ciphertext r0 = img * -8;
  Ciphertext r1 = img << -n - 1;
  Ciphertext r2 = img << -n;
  Ciphertext r3 = img << -n + 1;
  Ciphertext r4 = img << -1;
  Ciphertext r5 = img << 1;
  Ciphertext r6 = img << n - 1;
  Ciphertext r7 = img << n;
  Ciphertext r8 = img << n + 1;
  Ciphertext result = 2 * img - (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8);
  result.set_output("result");
}

int main(int argc, char **argv)
{
  int width = 32;
  if (argc > 1)
    width = stoi(argv[1]);

  int height = 32;
  if (argc > 2)
    height = stoi(argv[2]);

  int trs_passes = 1;
  if (argc > 3)
    trs_passes = stoi(argv[3]);

  bool optimize = trs_passes > 0;

  cout << "width: " << width << ", "
       << "height: " << height << ", "
       << "trs_passes: " << trs_passes << '\n';

  string func_name = "sharpening_filter";
  Compiler::create_func(func_name, height * width, 16, true, Scheme::bfv);
  sharpening_filter_opt(width, height);
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
