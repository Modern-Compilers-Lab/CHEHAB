#include "fheco/fheco.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void roberts_cross_opt()
{
  int w = 5;
  Ciphertext c0("c0", 0, 255);
  Ciphertext c0_rot_w = c0 >> w;
  Ciphertext c0_rot_1 = c0 >> 1;
  Ciphertext c1 = c0_rot_w - c0_rot_1;
  Ciphertext c2 = c1 * c1;
  Ciphertext c0_rot_6 = c0 >> (w + 1);
  Ciphertext c3 = c0 - c0_rot_6;
  Ciphertext c4 = c3 * c3;
  Ciphertext c_result = c2 + c4;
  c_result.set_output("c_result");
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{
  bool call_quantifier = false;
  if (argc > 1)
    call_quantifier = stoi(argv[1]);

  auto ruleset = Compiler::Ruleset::joined;
  if (argc > 2)
    ruleset = static_cast<Compiler::Ruleset>(stoi(argv[2]));

  auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
  if (argc > 3)
    rewrite_heuristic = static_cast<trs::RewriteHeuristic>(stoi(argv[3]));

  int64_t max_iter = 400000;
  if (argc > 4)
    max_iter = stoull(argv[4]);

  bool rewrite_created_sub_terms = true;
  if (argc > 5)
    rewrite_created_sub_terms = stoi(argv[5]);

  bool cse = true;
  if (argc > 6)
    cse = stoi(argv[6]);

  bool cse_order_operands = true;
  if (argc > 7)
    cse_order_operands = stoi(argv[7]);

  bool const_folding = true;
  if (argc > 8)
    const_folding = stoi(argv[8]);

  print_bool_arg(call_quantifier, "call_quantifier", clog);
  clog << " ";
  clog << ruleset << " " << rewrite_heuristic << " " << max_iter << " ";
  print_bool_arg(rewrite_created_sub_terms, "rewrite_created_sub_terms", clog);
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(cse_order_operands, "cse_order_operands", clog);
  clog << " ";
  print_bool_arg(const_folding, "const_folding", clog);
  clog << '\n';

  if (cse)
    Compiler::enable_cse();
  else
    Compiler::disable_cse();

  if (cse_order_operands)
    Compiler::enable_order_operands();
  else
    Compiler::disable_order_operands();

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  string func_name = "roberts_cross";
  // Compiler::create_func(func_name, 13, 16, true, true, false);
  Compiler::create_func(func_name, 13, 16, true, false, false);

  roberts_cross_opt();

  ofstream init_ir_os(func_name + "_init_ir.dot");
  util::draw_ir(Compiler::active_func(), init_ir_os);

  const auto &rand_inputs = Compiler::active_func()->data_flow().inputs_info();

  Compiler::compile(ruleset, rewrite_heuristic, max_iter, rewrite_created_sub_terms);

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
