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

void dot_product()
{
  Ciphertext c0("c0", -100, 100);
  Ciphertext c00("c00", -100, 100);
  Ciphertext c1 = c0 * c00;
  Ciphertext c1_rot_4 = c1 << 4;
  Ciphertext c2 = c1 + c1_rot_4;
  Ciphertext c2_rot_1 = c2 << 1;
  Ciphertext c3 = c2_rot_1 + c2;
  Ciphertext c3_rot_2 = c3 << 2;
  Ciphertext c_result = c3_rot_2 + c3;
  c_result.set_output("c_result");
}

void dot_product_plain()
{
  Ciphertext c0("c0", -100, 100);
  Plaintext v1("v1", -100, 100);
  Ciphertext c1 = c0 * v1;
  Ciphertext c1_rot_4 = c1 << 4;
  Ciphertext c2 = c1 + c1_rot_4;
  Ciphertext c2_rot_1 = c2 << 1;
  Ciphertext c3 = c2_rot_1 + c2;
  Ciphertext c3_rot_2 = c3 << 2;
  Ciphertext c_result = c3_rot_2 + c3;
  c_result.set_output("c_result");
}

int main(int argc, char **argv)
{
  size_t vector_size = 1024;
  if (argc > 1)
    vector_size = stoull(argv[1]);

  bool is_cipher_cipher = true;
  if (argc > 2)
  {
    std::stringstream ss(argv[2]);
    if (!(ss >> boolalpha >> is_cipher_cipher))
      throw invalid_argument("could not parse is_cipher_cipher to bools");
  }

  int trs_passes = 1;
  if (argc > 3)
    trs_passes = stoi(argv[3]);

  bool optimize = trs_passes > 0;

  cout << "vector_size: " << vector_size << ", "
       << "trs_passes: " << trs_passes << '\n';

  string func_name = "dot_product";
  Compiler::create_func(func_name, vector_size, 16, true, Scheme::bfv);
  if (is_cipher_cipher)
    dot_product();
  else
    dot_product_plain();
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
