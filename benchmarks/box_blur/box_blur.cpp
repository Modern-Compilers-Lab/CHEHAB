#include "fhecompiler.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void box_blur(int width, int height)
{
  Ciphertext img("img");
  Ciphertext top_row = img >> width;
  Ciphertext bottom_row = img << width;
  Ciphertext top_sum = top_row + (top_row >> 1) + (top_row << 1);
  Ciphertext curr_sum = img + (img >> 1) + (img << 1);
  Ciphertext bottom_sum = bottom_row + (bottom_row >> 1) + (bottom_row << 1);
  Ciphertext result = top_sum + curr_sum + bottom_sum;
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
       << "trs_passes: " << trs_passes << "\n";

  string func_name = "box_blur";
  Compiler::create_func(func_name, height * width, 16, false, Scheme::bfv);
  box_blur(width, height);
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
