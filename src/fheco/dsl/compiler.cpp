#include "fheco/code_gen/gen_func.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/passes.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/expr_printer.hpp"
#include "compiler.hpp"
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
  int vector_width = 0;
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
    vector_width += 1;
    expression = expression + " " + pr.terms_str_exprs().at(output_term->id()) + " ";
  }
  const char *env_var = std::getenv("VECTOR_WIDTH");

  if (env_var != nullptr)
  {
    std::string vector_width_env(env_var);
    int vector_width_env_int = std::stoi(vector_width_env);
    if (vector_width_env_int > vector_width)
    {
      for (int i = 0; i < vector_width_env_int - vector_width; i++)
      {
        expression = expression + " 0 ";
      }
      vector_width = vector_width_env_int;
    }
  }
  expression += ")";
  expression_file << expression;
  expression_file.close();
  ofstream vectorized_code_file("../vectorized_code.txt");
  vectorized_code_file << "";
  vectorized_code_file.close();
  call_vectorizer(vector_width);
  std::ofstream vectorized_code_file_2("../vectorized_code.txt", std::ios::app);
  vectorized_code_file_2 << vector_width << " " << vector_width;
  vectorized_code_file_2.close();
  call_script();
}
void Compiler::gen_vectorized_code(const std::shared_ptr<ir::Func> &func, int window)
{
  util ::ExprPrinter pr(func);

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
  int vector_full_width = 0;
  for (auto output_info : func->data_flow().outputs_info())
  {
    output_terms.push_back(output_info.first);
    vector_full_width += 1;
  }
  int sub_vectors_size = vector_full_width / window;
  int index = 0;
  string expression = "(Vec ";
  const char *env_var = std::getenv("VECTOR_WIDTH");
  int vector_width = 0;
  if (env_var != nullptr)
  {
    std::string vector_width_env(env_var);
    int vector_width = std::stoi(vector_width_env);
  }
  ofstream vectorized_code_file("../vectorized_code.txt");
  vectorized_code_file << "";
  vectorized_code_file.close();
  for (auto it = output_terms.rbegin(); it != output_terms.rend(); ++it)
  {
    auto output_term = *it;
    expression += pr.terms_str_exprs().at(output_term->id()) + " ";
    index = (index + 1) % sub_vectors_size;
    if ((!index && it != output_terms.rbegin()) || it == output_terms.rend() - 1)
    {
      int current_vector_width = (index == 0) ? sub_vectors_size : index + 1;
      for (int i = 0; i < vector_width - current_vector_width; i++)
      {
        expression += " 0 ";
        current_vector_width++;
      }
      expression += " )";
      ofstream expression_file("../expression.txt");
      expression_file << expression;
      expression_file.close();
      call_vectorizer(current_vector_width);
      expression = "(Vec ";
    }
  }
  std::ofstream vectorized_code_file_2("../vectorized_code.txt", std::ios::app);
  vectorized_code_file_2 << vector_full_width << " " << sub_vectors_size;
  vectorized_code_file_2.close();
  call_script();
}
void Compiler::call_vectorizer(int vector_width)
{
  string command = "cargo run --release --manifest-path ../../../egraphs/Cargo.toml -- ../expression.txt " +
                   to_string(vector_width) + " >> ../vectorized_code.txt";

  // Use the system function to run the executable
  int result = system(command.c_str());

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