#include "fhecompiler.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void hamming_distance(int vector_size)
{
  Ciphertext v1("v1");
  Ciphertext v2("v2");
  Ciphertext slot_wise_xor = v1 + v2 - 2 * (v1 * v2);
  Ciphertext result = reduce_add(slot_wise_xor);
  result.set_output("result");
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

  string func_name = "hamming_distance";
  Compiler::create_func(func_name, vector_size, 16, false, Scheme::bfv);
  hamming_distance(vector_size);
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
