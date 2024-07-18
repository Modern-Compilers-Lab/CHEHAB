#include "fheco/code_gen/gen_func.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/passes.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/expr_printer.hpp"
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;
using std::queue;
using std::string;
using std::vector;

namespace fheco
{
Compiler::FuncsTable Compiler::funcs_table_{};

Compiler::FuncsTable::const_iterator Compiler::active_func_it_ = Compiler::funcs_table_.cend();

bool Compiler::cse_enabled_ = false;

bool Compiler::order_operands_enabled_ = false;

bool Compiler::const_folding_enabled_ = false;

bool Compiler::scalar_vector_shape_ = true;

extern "C"
{
  void modify_string(char *str, size_t len);
}
void Compiler::compile(
  shared_ptr<ir::Func> func, Ruleset ruleset, trs::RewriteHeuristic rewrite_heuristic, ostream &header_os,
  string_view header_name, ostream &source_os, bool log2_reduct)
{
  auto rewrite_heuristicc = trs::RewriteHeuristic::bottom_up;
  trs::TRS joined_trs{trs::Ruleset::joined_ruleset(func)};
  joined_trs.run(rewrite_heuristicc);
  passes::cse_commut(func);
  gen_he_code(func, header_os, header_name, source_os, 29, true);
}

void Compiler::gen_he_code(
  const std::shared_ptr<ir::Func> &func, std::ostream &header_os, std::string_view header_name, std::ostream &source_os,
  size_t rotation_keys_threshold, bool lazy_relin)
{
#ifdef FHECO_LOGGING
  clog << "\nrotation_key_selection\n";
#endif
  unordered_set<int> rotation_steps_keys;
  rotation_steps_keys = passes::reduce_rotation_keys(func, rotation_keys_threshold);

#ifdef FHECO_LOGGING
  clog << "\nrelin_insertion\n";
#endif
  size_t relin_keys_count;
  if (lazy_relin)
    relin_keys_count = passes::lazy_relin_heuristic(func);
  else
    relin_keys_count = passes::relin_after_ctxt_ctxt_mul(func);

#ifdef FHECO_LOGGING
  clog << "\ncode_generation\n";
#endif
  code_gen::gen_func(func, rotation_steps_keys, header_os, header_name, source_os);
}

const shared_ptr<ir::Func> &Compiler::add_func(shared_ptr<ir::Func> func)
{
  if (auto it = funcs_table_.find(func->name()); it != funcs_table_.end())
    throw invalid_argument("function with this name already exists");

  active_func_it_ = funcs_table_.emplace(func->name(), move(func)).first;
  return active_func_it_->second;
}

const shared_ptr<ir::Func> &Compiler::get_func(const string &name)
{
  auto it = funcs_table_.find(name);
  if (it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  return it->second;
}

void Compiler::set_active_func(const string &name)
{
  active_func_it_ = funcs_table_.find(name);
  if (active_func_it_ == funcs_table_.cend())
    throw invalid_argument("no function with this name was found");
}

void Compiler::delete_func(const string &name)
{
  if (active_func()->name() == name)
    active_func_it_ = funcs_table_.end();
  funcs_table_.erase(name);
}

ostream &operator<<(ostream &os, Compiler::Ruleset ruleset)
{
  switch (ruleset)
  {
  case Compiler::Ruleset::depth:
    os << "depth";
    break;

  case Compiler::Ruleset::ops_cost:
    os << "ops_cost";
    break;

  case Compiler::Ruleset::joined:
    os << "joined";
    break;

  default:
    throw invalid_argument("invalid ruleset selector");
    break;
  }

  return os;
}
void Compiler::gen_vectorized_code(const std::shared_ptr<ir::Func> &func)
{

  util ::ExprPrinter pr(func);
  int slot_count = 0;
  pr.make_terms_str_expr(util::ExprPrinter::Mode::prefix);
  ofstream inputs_file("../inputs.txt");
  string inputs_names = "";
  string inputs_type = "";
  vector<const ir::Term *> input_terms;
  for (auto input_info : func->data_flow().inputs_info())
  {
    input_terms.push_back(input_info.first);
  }
  for (auto it = input_terms.rbegin(); it != input_terms.rend(); ++it)
  {
    auto input_term = *it;
    inputs_names += pr.terms_str_exprs().at(input_term->id()) + " ";
    if (input_term->type() == ir::Term::Type::cipher)
    {
      inputs_type += "1 ";
    }
    else
      inputs_type += "0 ";
  }
  inputs_file << inputs_names << endl;
  inputs_file << inputs_type << endl;
  inputs_file.close();
  vector<const ir::Term *> output_terms;
  for (auto output_info : func->data_flow().outputs_info())
  {
    output_terms.push_back(output_info.first);
  }
  ofstream expression_file("../expression.txt");
  string expression = "(Vec ";
  for (auto it = output_terms.rbegin(); it != output_terms.rend(); ++it)
  {
    auto output_term = *it;
    slot_count += 1;
    expression = expression + " " + pr.terms_str_exprs().at(output_term->id()) + " ";
  }

  expression += ")";
  expression_file << expression;
  expression_file.close();
  if (setenv("VECTOR_WIDTH", to_string(slot_count).c_str(), 1) != 0)
  {
    std::cerr << "Failed to set environment variable" << std::endl;
    exit(1);
  }
  call_vectorizer();
  call_script();
}
void Compiler::call_vectorizer()
{
  const char *command = "cargo run --release --manifest-path ../../../egraphs/Cargo.toml -- ../expression.txt "
                        "--no-ac > ../vectorized_code.txt";

  // Use the system function to run the executable
  int result = system(command);

  // Check the result of the system call
  if (result != 0)
  {
    // The executable did not run successfully
    std::cout << "Failed to call the vectorizer engine!" << std::endl;
  }
}

void Compiler::call_script()
{
  const char *command = "python3 ../script.py ";
  int result = system(command);
  if (result != 0)
  {
    // The executable did not run successfully
    std::cout << "Failed to call the script " << std::endl;
  }
}
} // namespace fheco