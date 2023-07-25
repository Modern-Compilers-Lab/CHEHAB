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

void cryptonets(integer modulus)
{
  // shapes
  vector<size_t> x_shape = {28, 28, 1};
  vector<size_t> w1_shape = {5, 5, 1, 5};
  vector<size_t> b1_shape = {5};
  vector<size_t> w4_shape = {5, 5, 5, 10};
  vector<size_t> b4_shape = {10};
  vector<size_t> w8_shape = {40, 10};
  vector<size_t> b8_shape = {10};
  // load constants
  string constants_loc = "constants/plain_mod_" + to_string(modulus) + "/";
  ifstream w1_is(constants_loc + "w1.txt");
  if (!w1_is)
    throw invalid_argument("failed to open w1 file");

  ifstream w4_is(constants_loc + "w4.txt");
  if (!w4_is)
    throw invalid_argument("failed to open w4 file");

  ifstream w8_is(constants_loc + "w8.txt");
  if (!w8_is)
    throw invalid_argument("failed to open w8 file");

  ifstream b1_is(constants_loc + "b1.txt");
  if (!b1_is)
    throw invalid_argument("failed to open b1 file");

  ifstream b4_is(constants_loc + "b4.txt");
  if (!b4_is)
    throw invalid_argument("failed to open b4 file");

  ifstream b8_is(constants_loc + "b8.txt");
  if (!b8_is)
    throw invalid_argument("failed to open b8 file");

  char delim = ' ';
  vector<vector<vector<vector<integer>>>> w1_vals;
  {
    auto w1_raw = load(w1_is, delim);
    w1_vals = reshape_4d(w1_raw, w1_shape);
  }
  vector<vector<vector<vector<integer>>>> w4_vals;
  {
    auto w4_raw = load(w4_is, delim);
    w4_vals = reshape_4d(w4_raw, w4_shape);
  }
  auto w8_vals = load(w8_is, delim);
  auto b1_vals = load(b1_is);
  auto b4_vals = load(b4_is);
  auto b8_vals = load(b8_is);
  // declare constants
  vector<vector<vector<vector<Plaintext>>>> w1(
    w1_shape[0], vector<vector<vector<Plaintext>>>(
                   w1_shape[1], vector<vector<Plaintext>>(w1_shape[2], vector<Plaintext>(w1_shape[3]))));
  for (size_t i = 0; i < w1_shape[0]; ++i)
    for (size_t j = 0; j < w1_shape[1]; ++j)
      for (size_t k = 0; k < w1_shape[2]; ++k)
        for (size_t l = 0; l < w1_shape[3]; ++l)
          w1[i][j][k][l] = w1_vals[i][j][k][l];

  vector<vector<vector<vector<Plaintext>>>> w4(
    w4_shape[0], vector<vector<vector<Plaintext>>>(
                   w4_shape[1], vector<vector<Plaintext>>(w4_shape[2], vector<Plaintext>(w4_shape[3]))));
  for (size_t i = 0; i < w4_shape[0]; ++i)
    for (size_t j = 0; j < w4_shape[1]; ++j)
      for (size_t k = 0; k < w4_shape[2]; ++k)
        for (size_t l = 0; l < w4_shape[3]; ++l)
          w4[i][j][k][l] = w4_vals[i][j][k][l];

  vector<vector<Plaintext>> w8(w8_shape[0], vector<Plaintext>(w8_shape[1]));
  for (size_t i = 0; i < w8_shape[0]; ++i)
    for (size_t j = 0; j < w8_shape[1]; ++j)
      w8[i][j] = w8_vals[i][j];

  vector<Plaintext> b1(b1_shape[0]);
  for (size_t i = 0; i < b1_shape[0]; ++i)
    b1[i] = b1_vals[i];

  vector<Plaintext> b4(b4_shape[0]);
  for (size_t i = 0; i < b4_shape[0]; ++i)
    b4[i] = b4_vals[i];

  vector<Plaintext> b8(b8_shape[0]);
  for (size_t i = 0; i < b8_shape[0]; ++i)
    b8[i] = b8_vals[i];

  // declare inputs
  integer x_min_val = 0;
  integer x_max_val = 100;
  vector<vector<vector<Ciphertext>>> x(
    x_shape[0], vector<vector<Ciphertext>>(x_shape[1], vector<Ciphertext>(x_shape[2])));
  for (size_t i = 0; i < x_shape[0]; ++i)
    for (size_t j = 0; j < x_shape[1]; ++j)
      for (size_t k = 0; k < x_shape[2]; ++k)
        x[i][j][k] =
          Ciphertext("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]", x_min_val, x_max_val);

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
  integer modulus = 65537;
  if (argc > 1)
    modulus = static_cast<integer>(stoull(argv[1]));

  bool call_quantifier = false;
  if (argc > 2)
    call_quantifier = stoi(argv[2]);

  auto ruleset = Compiler::Ruleset::joined;
  if (argc > 3)
    ruleset = static_cast<Compiler::Ruleset>(stoi(argv[3]));

  auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
  if (argc > 4)
    rewrite_heuristic = static_cast<trs::RewriteHeuristic>(stoi(argv[4]));

  bool cse = true;
  if (argc > 5)
    cse = stoi(argv[5]);

  bool const_folding = true;
  if (argc > 6)
    const_folding = stoi(argv[6]);

  clog << modulus;
  clog << " ";
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

  string app_name = "cryptonets_" + to_string(modulus);
  size_t slot_count = 8192;
  bool delayed_reduct = false;
  bool signdness = false;
  bool need_cyclic_rotation = false;

  clog << "\nnoopt function\n";
  string noopt_func_name = app_name + "_noopt";
  const auto &noopt_func =
    Compiler::create_func(noopt_func_name, slot_count, delayed_reduct, modulus, signdness, need_cyclic_rotation);
  cryptonets(modulus);

  string noopt_gen_name = "gen_he_" + noopt_func_name;
  string noopt_gen_path = "he/gen/" + noopt_gen_name;
  ofstream noopt_header_os(noopt_gen_path + ".hpp");
  if (!noopt_header_os)
    throw logic_error("failed to create noopt_header file");

  ofstream noopt_source_os(noopt_gen_path + ".cpp");
  if (!noopt_source_os)
    throw logic_error("failed to create noopt_source file");

  Compiler::gen_he_code(noopt_func, noopt_header_os, noopt_gen_name + ".hpp", noopt_source_os);

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
  const auto &opt_func =
    Compiler::create_func(opt_func_name, slot_count, delayed_reduct, modulus, signdness, need_cyclic_rotation);
  cryptonets(modulus);

  string opt_gen_name = "gen_he_" + opt_func_name;
  string opt_gen_path = "he/gen/" + opt_gen_name;
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

  ofstream io_example_os("io_examples/" + app_name + "_io_example.txt");
  if (!io_example_os)
    throw logic_error("failed to create io_example file");

  util::print_io_terms_values(noopt_func, io_example_os);
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
