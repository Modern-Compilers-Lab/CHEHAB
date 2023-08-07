#include "fheco/fheco.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void poly_reg()
{
  Ciphertext c0("c0", -10, 10);
  Ciphertext c1("c1", -10, 10);
  Ciphertext c2("c2", -10, 10);
  Ciphertext c3("c3", -10, 10);
  Ciphertext c4("c4", -10, 10);
  Ciphertext c_result = c1 - (c0 * c0 * c4 + c0 * c3 + c2);
  c_result.set_output("c_result");
}

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

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{
  bool call_quantifier = false;
  if (argc > 1)
    call_quantifier = stoi(argv[1]);

  auto ruleset = Compiler::Ruleset::ops_cost;
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

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  clog << "trs_" << ruleset << " " << rewrite_heuristic;
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "const_folding", clog);
  clog << '\n';

  string app_name = "poly_reg";
  size_t slot_count = 1024;
  int bit_width = 20;
  bool signdness = true;
  bool need_cyclic_rotation = false;

  clog << "\nnoopt function\n";
  string noopt_func_name = app_name + "_noopt";
  const auto &noopt_func =
    Compiler::create_func(noopt_func_name, slot_count, bit_width, signdness, need_cyclic_rotation);
  poly_reg();

  string noopt_gen_name = "gen_he_" + noopt_func_name;
  string noopt_gen_path = "he/" + noopt_gen_name;
  ofstream noopt_header_os(noopt_gen_path + ".hpp");
  if (!noopt_header_os)
    throw logic_error("failed to create noopt_header file");

  ofstream noopt_source_os(noopt_gen_path + ".cpp");
  if (!noopt_source_os)
    throw logic_error("failed to create noopt_source file");

  Compiler::gen_he_code(noopt_func, noopt_header_os, noopt_gen_name + ".hpp", noopt_source_os);

  ofstream noopt_ir_os(noopt_func_name + "_ir.dot");
  if (!noopt_ir_os)
    throw logic_error("failed to create noopt_ir file");

  util::draw_ir(noopt_func, noopt_ir_os);
  util::Quantifier noopt_quantifier(noopt_func);
  if (call_quantifier)
  {
    cout << "\ninitial circuit characteristics\n";
    noopt_quantifier.run_all_analysis();
    noopt_quantifier.print_info(cout);
    cout << endl;
  }

  clog << "\nopt function\n";
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

  string opt_func_name = app_name + "_opt";
  const auto &opt_func = Compiler::create_func(opt_func_name, slot_count, bit_width, signdness, need_cyclic_rotation);
  poly_reg();

  string opt_gen_name = "gen_he_" + opt_func_name;
  string opt_gen_path = "he/" + opt_gen_name;
  ofstream opt_header_os(opt_gen_path + ".hpp");
  if (!opt_header_os)
    throw logic_error("failed to create opt_header file");

  ofstream opt_source_os(opt_gen_path + ".cpp");
  if (!opt_source_os)
    throw logic_error("failed to create opt_source file");

  Compiler::compile(opt_func, ruleset, rewrite_heuristic, opt_header_os, opt_gen_name + ".hpp", opt_source_os);

  auto noopt_obtained_outputs = util::evaluate_on_clear(noopt_func, opt_func->get_inputs_example_values());
  auto opt_obtained_outputs = util::evaluate_on_clear(opt_func, noopt_func->get_inputs_example_values());
  if (
    noopt_obtained_outputs != opt_func->get_outputs_example_values() ||
    opt_obtained_outputs != noopt_func->get_outputs_example_values())
    throw logic_error("compilation correctness-test failed");

  ofstream io_example_os(app_name + "_io_example.txt");
  if (!io_example_os)
    throw logic_error("failed to create io_example file");

  util::print_io_terms_values(noopt_func, io_example_os);
  ofstream opt_ir_os(opt_func_name + "_ir.dot");
  if (!opt_ir_os)
    throw logic_error("failed to create opt_ir file");

  util::draw_ir(opt_func, opt_ir_os);
  if (call_quantifier)
  {
    cout << "\nfinal circuit characteristics\n";
    util::Quantifier opt_quantifier(opt_func);
    opt_quantifier.run_all_analysis();
    opt_quantifier.print_info(cout);

    cout << "\nimprovement rates\n";
    auto diff_quantifier = (noopt_quantifier - opt_quantifier) / noopt_quantifier * 100;
    diff_quantifier.print_info(cout);
  }
  return 0;
}
