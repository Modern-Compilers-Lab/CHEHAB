#include "fheco/fheco.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
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
  vector<vector<vector<Ciphertext>>> x(
    x_shape[0], vector<vector<Ciphertext>>(x_shape[1], vector<Ciphertext>(x_shape[2])));
  for (size_t i = 0; i < x_shape[0]; ++i)
    for (size_t j = 0; j < x_shape[1]; ++j)
      for (size_t k = 0; k < x_shape[2]; ++k)
        x[i][j][k] = Ciphertext("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]");

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

  auto axiomatic = false;
  if (argc > 2)
    axiomatic = stoi(argv[2]) ? true : false;

  auto window = 0;
  if (argc > 3)
    window = stoi(argv[3]);

  bool call_quantifier = false;
  if (argc > 4)
    call_quantifier = stoi(argv[4]);

  bool cse = true;
  if (argc > 5)
    cse = stoi(argv[5]);

  bool const_folding = true;
  if (argc > 6)
    const_folding = stoi(argv[6]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
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

  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  t = chrono::high_resolution_clock::now();
  string func_name = "cryptonets_" + to_string(modulus);
  const auto &func = Compiler::create_func(func_name, 8192, false, modulus, false, false);
  cryptonets(modulus);

  string gen_name = "_gen_he_" + func_name;
  string gen_path = "he/gen/" + gen_name;
  ofstream header_os(gen_path + ".hpp");
  if (!header_os)
    throw logic_error("failed to create header file");

  ofstream source_os(gen_path + ".cpp");
  if (!source_os)
    throw logic_error("failed to create source file");

  Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
  elapsed = chrono::high_resolution_clock::now() - t;
  cout << elapsed.count() << " ms\n";

  if (call_quantifier)
  {
    util::Quantifier quantifier{func};
    quantifier.run_all_analysis();
    quantifier.print_info(cout);
  }
  return 0;
}
