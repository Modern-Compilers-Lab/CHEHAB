#include "fhecompiler/fhecompiler.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

int main()
{
  fhecompiler::Compiler::create_func("box_blur", 8);

  fhecompiler::Ciphertext c0("c0", fhecompiler::VarType::input);

  fhecompiler::Ciphertext c1 = c0 << 1;
  fhecompiler::Ciphertext c2 = c0 << 5;
  fhecompiler::Ciphertext c3 = c0 << 6;
  fhecompiler::Ciphertext c4 = c1 + c0;
  fhecompiler::Ciphertext c5 = c2 + c3;
  fhecompiler::Ciphertext c6 = c4 + c5;
  fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
  output = c6;

  vector<int64_t> random_c0(8);
  utils::init_random(random_c0, -100, 100);

  utils::variables_values_map inputs_values = {{c0.get_label(), random_c0}};

  utils::print_variables_values(inputs_values);

  auto clear_outputs1 = fhecompiler::Compiler::evaluate_on_clear(inputs_values);

  fhecompiler::Compiler::compile("box_blur.hpp");

  auto clear_outputs2 = fhecompiler::Compiler::evaluate_on_clear(inputs_values);

  if (clear_outputs1 != clear_outputs2)
    throw logic_error("clear_outputs1 != clear_outputs2");

  utils::print_variables_values(clear_outputs2);

  return 0;
}
