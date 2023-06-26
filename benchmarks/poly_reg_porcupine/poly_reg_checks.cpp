#include "fheco/fheco.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void poly_reg_baseline()
{
  Ciphertext c0("c0", -10, 10);
  Ciphertext c1("c1", -10, 10);
  Ciphertext c2("c2", -10, 10);
  Ciphertext c3("c3", -10, 10);
  Ciphertext c4("c4", -10, 10);
  Ciphertext c_result = c1 - (square(c0) * c4 + c0 * c3 + c2);
  c_result.set_output("c_result");
}

void poly_reg_synthesized()
{
  Ciphertext c0("c0", -10, 10);
  Ciphertext c1("c1", -10, 10);
  Ciphertext c2("c2", -10, 10);
  Ciphertext c3("c3", -10, 10);
  Ciphertext c4("c4", -10, 10);
  Ciphertext c5 = c0 * c4;
  Ciphertext c6 = c5 + c3;
  Ciphertext c7 = c0 * c6;
  Ciphertext c8 = c7 + c2;
  Ciphertext c_result = c1 - c8;
  c_result.set_output("c_result");
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "non_" + name);
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

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

  print_bool_arg(call_quantifier, "quantificateur", clog);
  clog << " ";
  clog << "trs_" << ruleset << " " << rewrite_heuristic;
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "const_folding", clog);
  clog << '\n';

  if (cse)
  {
    Compiler::enable_cse();
    Compiler::enable_order_operands();
  }
  else
  {
    Compiler::disable_cse();
    Compiler::disable_order_operands();
  }

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  string func_name = "poly_reg";
  const auto &func = Compiler::create_func(func_name, 1024, 20, true, false);

  poly_reg_baseline();

  util::Quantifier init_quantifier(func);
  if (call_quantifier)
  {
    cout << "\ncaractéristiques du circuit initial\n";
    init_quantifier.run_all_analysis();
    init_quantifier.print_info(cout);
    cout << endl;
  }

  ofstream init_ir_os(func_name + "_init_ir.dot");
  util::draw_ir(func, init_ir_os);

  const auto &rand_inputs = func->data_flow().inputs_info();

  string gen_name = "gen_he_" + func_name;
  string gen_path = "he/" + gen_name;
  ofstream header_os(gen_path + ".hpp");
  ofstream source_os(gen_path + ".cpp");

  Compiler::compile(ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);

  auto outputs = util::evaluate_on_clear(func, rand_inputs);
  if (outputs != func->data_flow().outputs_info())
    throw logic_error("compilation correctness-test failed");

  ofstream rand_example_os(func_name + "_rand_example.txt");
  util::print_io_terms_values(func, rand_example_os);

  ofstream final_ir_os(func_name + "_final_ir.dot");
  util::draw_ir(func, final_ir_os);

  if (call_quantifier)
  {
    cout << "\ncaractéristiques du circuit final\n";
    util::Quantifier final_quantifier(func);
    final_quantifier.run_all_analysis();
    final_quantifier.print_info(cout);

    cout << "\ntaux d'amélioration\n";
    auto diff_quantifier = (init_quantifier - final_quantifier) / init_quantifier * 100;
    diff_quantifier.print_info(cout);
  }

  return 0;
}
