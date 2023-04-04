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

void roberts_cross(int width, int height)
{
  Ciphertext img("img", 0, 255);
  Ciphertext bottom_row = img << width;
  // Gx
  Ciphertext gx = img - (bottom_row << 1);
  // Gy
  Ciphertext gy = (img << 1) - bottom_row;
  // combine
  Ciphertext result = square(gx) + square(gy);
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

  string func_name = "roberts_cross";
  Compiler::create_func(func_name, height * width, 20, true, Scheme::bfv);
  roberts_cross(width, height);
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
