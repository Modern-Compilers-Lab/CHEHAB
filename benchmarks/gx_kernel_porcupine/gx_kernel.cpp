#include "fheco/fheco.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void gx_kernel_baseline()
{
  Ciphertext c0("c0", 0, 255);
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
  c12.set_output("c12");
}

void gx_kernel_synthesized()
{
  Ciphertext c0("c0", 0, 255);
  Ciphertext c1 = c0 << -5;
  Ciphertext c2 = c0 + c1;
  Ciphertext c3 = c2 << 5;
  Ciphertext c4 = c3 + c2;
  Ciphertext c5 = c4 << -1;
  Ciphertext c6 = c4 << 1;
  Ciphertext c7 = c6 - c5;
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{
  auto axiomatic = false;
  if (argc > 1)
    axiomatic = stoi(argv[1]) ? true : false;

  auto window = 0;
  if (argc > 2)
    window = stoi(argv[2]);

  bool call_quantifier = false;
  if (argc > 3)
    call_quantifier = stoi(argv[3]);

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
  clog << '\n';

  if (cse)
    Compiler::enable_cse();
  else
    Compiler::disable_cse();

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  print_bool_arg(call_quantifier, "call_quantifier", clog);
  clog << " ";

  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";

  print_bool_arg(const_folding, "const_folding", clog);
  clog << '\n';

  string func_name = "gx_kernel";
  Compiler::create_func(func_name, 13, 20, true, false, false);
  // Compiler::create_func(func_name, 16, 20, true, true, false);

  gx_kernel_baseline();

  ofstream init_ir_os(func_name + "_init_ir.dot");
  util::draw_ir(Compiler::active_func(), init_ir_os);

  const auto &rand_inputs = Compiler::active_func()->data_flow().inputs_info();

  Compiler::compile(func, header_os, gen_name + ".hpp", source_os, axiomatic, window);

  auto outputs = util::evaluate_on_clear(Compiler::active_func(), rand_inputs);
  if (outputs != Compiler::active_func()->data_flow().outputs_info())
    throw logic_error("compilation correctness-test failed");

  ofstream rand_example_os(func_name + "_rand_example.txt");
  util::print_io_terms_values(Compiler::active_func(), rand_example_os);

  ofstream final_ir_os(func_name + "_final_ir.dot");
  util::draw_ir(Compiler::active_func(), final_ir_os);

  if (call_quantifier)
  {
    util::Quantifier quantifier1(Compiler::active_func());
    quantifier1.run_all_analysis();
    quantifier1.print_info(cout);
  }
  return 0;
}
