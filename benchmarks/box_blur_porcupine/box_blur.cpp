#include "fhecompiler/fhecompiler.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void box_blur()
{
  Ciphertext c0("c0", VarType::input);
  Ciphertext c1 = c0 << 1;
  Ciphertext c2 = c0 << 5;
  Ciphertext c3 = c0 << 6;
  Ciphertext c4 = c1 + c0;
  Ciphertext c5 = c2 + c3;
  Ciphertext c6("c6", VarType::output);
  c6 = c4 + c5;
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
       << "trs_passes: " << trs_passes << "\n";

  string func_name = "box_blur";
  Compiler::create_func(func_name, vector_size, 16, false, Scheme::bfv);
  box_blur();
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
