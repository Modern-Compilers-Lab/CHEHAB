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
  string func_name = "box_blur";
  Compiler::create_func(func_name, 8);

  Ciphertext c0("c0", VarType::input);

  Ciphertext c1 = c0 << 1;
  Ciphertext c2 = c0 << 5;
  Ciphertext c3 = c0 << 6;
  Ciphertext c4 = c1 + c0;
  Ciphertext c5 = c2 + c3;
  Ciphertext c6 = c4 + c5;
  Ciphertext output("output", VarType::output);
  output = c6;

  const utils::variables_values_map &inputs_values = Compiler::get_input_values();

  auto clear_outputs1 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != Compiler::get_output_values())
    throw logic_error("clear_outputs1 != Compiler::get_output_values()");

  Compiler::compile(func_name + ".hpp");

  auto clear_outputs2 = Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != clear_outputs2)
    throw logic_error("clear_outputs1 != clear_outputs2");

  Compiler::serialize_inputs_outputs(func_name + ".txt");

  return 0;
}
