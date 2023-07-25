#include "fheco/fheco.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ml.hpp"

using namespace std;
using namespace fheco;

void cryptonets(
  const vector<size_t> &x_shape, const vector<size_t> &w1_shape, const vector<size_t> &b1_shape,
  const vector<size_t> &w4_shape, const vector<size_t> &b4_shape, const vector<size_t> &w8_shape,
  const vector<size_t> &b8_shape)
{
  // declare inputs
  int x_min_val = -10;
  int x_max_val = 10;
  int wb_min = -10;
  int wb_max = 10;
  vector<vector<vector<Ciphertext>>> x;
  for (size_t i = 0; i < x_shape[0]; ++i)
  {
    x.push_back(vector<vector<Ciphertext>>());
    for (size_t j = 0; j < x_shape[1]; ++j)
    {
      x[i].push_back(vector<Ciphertext>());
      for (size_t k = 0; k < x_shape[2]; ++k)
        x[i][j].push_back(
          Ciphertext("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]", x_min_val, x_max_val));
    }
  }

  vector<vector<vector<vector<Plaintext>>>> w1;
  for (size_t i = 0; i < w1_shape[0]; ++i)
  {
    w1.push_back(vector<vector<vector<Plaintext>>>());
    for (size_t j = 0; j < w1_shape[1]; ++j)
    {
      w1[i].push_back(vector<vector<Plaintext>>());
      for (size_t k = 0; k < w1_shape[2]; ++k)
      {
        w1[i][j].push_back(vector<Plaintext>());
        for (size_t l = 0; l < w1_shape[3]; ++l)
          w1[i][j][k].push_back(Plaintext(
            "w1[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]", wb_min,
            wb_max));
      }
    }
  }

  vector<Plaintext> b1;
  for (size_t i = 0; i < b1_shape[0]; ++i)
    b1.push_back(Plaintext("b1[" + to_string(i) + "]", wb_min, wb_max));

  vector<vector<vector<vector<Plaintext>>>> w4;
  for (size_t i = 0; i < w4_shape[0]; ++i)
  {
    w4.push_back(vector<vector<vector<Plaintext>>>());
    for (size_t j = 0; j < w4_shape[1]; ++j)
    {
      w4[i].push_back(vector<vector<Plaintext>>());
      for (size_t k = 0; k < w4_shape[2]; ++k)
      {
        w4[i][j].push_back(vector<Plaintext>());
        for (size_t l = 0; l < w4_shape[3]; ++l)
          w4[i][j][k].push_back(Plaintext(
            "w4[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]", wb_min,
            wb_max));
      }
    }
  }

  vector<Plaintext> b4;
  for (size_t i = 0; i < b4_shape[0]; ++i)
    b4.push_back(Plaintext("b4[" + to_string(i) + "]", wb_min, wb_max));

  vector<vector<Plaintext>> w8;
  for (size_t i = 0; i < w8_shape[0]; ++i)
  {
    w8.push_back(vector<Plaintext>());
    for (size_t j = 0; j < w8_shape[1]; ++j)
      w8[i].push_back(Plaintext("w8[" + to_string(i) + "][" + to_string(j) + "]", wb_min, wb_max));
  }

  vector<Plaintext> b8;
  for (size_t i = 0; i < b8_shape[0]; ++i)
    b8.push_back(Plaintext("b8[" + to_string(i) + "]", wb_min, wb_max));

  // predict
  auto y = predict(x, w1, b1, w4, b4, w8, b8);
  // declare outputs
  for (size_t i = 0; i < y.size(); ++i)
    y[i].set_output("y[" + to_string(i) + "]");
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

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  clog << ruleset << "_trs";
  clog << " ";
  clog << rewrite_heuristic;
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
  clog << '\n';

  string app_name = "cryptonets";
  size_t slot_count = 8192;
  int bit_width = 17;
  bool signdness = true;
  bool need_cyclic_rotation = false;

  clog << "\nnoopt function\n";

  string noopt_func_name = app_name + "_noopt";
  const auto &noopt_func =
    Compiler::create_func(noopt_func_name, slot_count, bit_width, signdness, need_cyclic_rotation);

  cryptonets(
    vector<size_t>{28, 28, 1}, vector<size_t>{5, 5, 1, 5}, vector<size_t>{5}, vector<size_t>{5, 5, 5, 10},
    vector<size_t>{10}, vector<size_t>{40, 10}, vector<size_t>{10});

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

  cryptonets(
    vector<size_t>{28, 28, 1}, vector<size_t>{5, 5, 1, 5}, vector<size_t>{5}, vector<size_t>{5, 5, 5, 10},
    vector<size_t>{10}, vector<size_t>{40, 10}, vector<size_t>{10});

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
