#include "fheco/fheco.hpp"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace fheco;

void hamming_distance_baseline()
{
  Plaintext d0(2);
  Ciphertext c0("c0");
  Ciphertext c1("c1");
  Ciphertext c2 = c1 + c0;
  Ciphertext c3 = -c2 * c1 - c2 * c0 + c2 * d0;
  Ciphertext c3_rot_2 = c3 << 2;
  Ciphertext c4 = c3_rot_2 + c3;
  Ciphertext c4_rot_1 = c4 << 1;
  Ciphertext c_result = c4_rot_1 + c4;

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

  chrono::high_resolution_clock::time_point time_start, time_end;
  chrono::duration<double, milli> time_sum(0);
  time_start = chrono::high_resolution_clock::now();
  string func_name = "hamming_dist";
  const auto &func = Compiler::create_func(func_name, 4, 20, true, false);

  hamming_distance_baseline();

  string gen_name = "_gen_he_" + func_name;
  string gen_path = "he/" + gen_name;
  ofstream header_os(gen_path + ".hpp");
  ofstream source_os(gen_path + ".cpp");

  Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);

  time_end = chrono::high_resolution_clock::now();
  time_sum = time_end - time_start;
  cout << time_sum.count() << '\n';

  if (call_quantifier)
  {
    util::Quantifier quantifier{func};
    quantifier.run_all_analysis();
    quantifier.print_info(cout);
  }

  return 0;
}
