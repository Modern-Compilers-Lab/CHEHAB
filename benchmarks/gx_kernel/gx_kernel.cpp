#include "fhecompiler/fhecompiler.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

int main()
{
  string func_name = "gx_kernel";
  Compiler::create_func(func_name, 8);

  Ciphertext c0("c0", VarType::input);

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
  Ciphertext output("output", VarType::output);
  output = c12;

  const utils::variables_values_map &inputs_values = Compiler::get_input_values();

  auto clear_outputs1 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != Compiler::get_output_values())
    throw logic_error("clear_outputs1 != Compiler::get_output_values()");

  Compiler::compile("he/gen_he_" + func_name + ".hpp");

  auto clear_outputs2 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != clear_outputs2)
    throw logic_error("clear_outputs1 != clear_outputs2");

  Compiler::serialize_inputs_outputs(func_name + "_rand_example.txt");

  return 0;
}
