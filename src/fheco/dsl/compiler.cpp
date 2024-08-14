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
/**
 * Generates vectorized code for a given function.
 *
 * This function performs the following tasks:
 * 1. Initializes an expression printer in prefix notation to print expressions.
 * 2. Opens three output files: inputs.txt, expression.txt, and vectorized_code.txt.
 * 3. Checks if the files opened successfully.
 * 4. Processes the input terms, storing their names and types in separate strings.
 * 5. Writes the input names and types to inputs.txt.
 * 6. Processes the output terms to form an expression that represents the scalar computation.
 * 7. Computes the vector width based on the number of output terms.
 * 8. Reads the VECTOR_WIDTH environment variable to possibly extend the vector width.
 * 9. Pads the expression with zeros if the environment variable indicates a larger vector size.
 * 10. Writes the scalar expression to expression.txt.
 * 11. Clears the content of vectorized_code.txt to prepare for the vectorized code.
 * 12. Closes the input and expression files.
 * 13. Calls the vectorizer function to generate the vectorized code.
 * 14. Appends the vector width to vectorized_code.txt.
 * 15. Calls a python script to build the source code that operates on vectors.
 *
 * @param func Shared pointer to the function to be vectorized.
 */
void Compiler::gen_vectorized_code(const std::shared_ptr<ir::Func> &func)
{
  // Utility function to print expressions in prefix notation
  util::ExprPrinter expr_printer(func);
  expr_printer.make_terms_str_expr(util::ExprPrinter::Mode::prefix);

  // Initialize files to store inputs, expression, and vectorized code
  std::ofstream inputs_file("../inputs.txt");
  std::ofstream expression_file("../expression.txt");
  std::ofstream vectorized_code_file("../vectorized_code.txt");

  // Check if any of the files failed to open
  if (!inputs_file || !expression_file || !vectorized_code_file)
  {
    std::cerr << "Error opening one of the output files." << std::endl;
    return;
  }

  // Strings to store input names and types
  std::string input_names;
  std::string input_types;
  int vector_width = 0;

  // Helper function to process input terms and store their names and types
  auto process_input_terms = [&](const ir::InputTermsInfo &inputs_info) {
    std::vector<const ir::Term *> input_terms;
    for (const auto &input_info : inputs_info)
    {
      input_terms.push_back(input_info.first);
    }
    for (auto it = input_terms.rbegin(); it != input_terms.rend(); ++it)
    {
      auto input_term = *it;
      input_names += expr_printer.terms_str_exprs().at(input_term->id()) + " ";
      input_types += (input_term->type() == ir::Term::Type::cipher) ? "1 " : "0 ";
    }
  };

  // Process input terms and write to inputs_file
  process_input_terms(func->data_flow().inputs_info());
  inputs_file << input_names << std::endl;
  inputs_file << input_types << std::endl;

  // Helper function to process output terms and return them as a vector
  auto process_output_terms = [&](const ir::OutputTermsInfo &outputs_info) {
    std::vector<const ir::Term *> output_terms;
    for (const auto &output_info : outputs_info)
    {
      output_terms.push_back(output_info.first);
    }
    return output_terms;
  };

  // Process output terms
  std::vector<const ir::Term *> output_terms = process_output_terms(func->data_flow().outputs_info());
  std::string expression = "(Vec ";
  for (const auto &output_term : output_terms)
  {
    expression += expr_printer.terms_str_exprs().at(output_term->id()) + " ";
    ++vector_width;
  }

  // Read and parse the VECTOR_WIDTH environment variable
  const char *env_var = std::getenv("VECTOR_WIDTH");
  if (env_var)
  {
    try
    {
      int vector_width_env = std::stoi(env_var);
      while (vector_width < vector_width_env)
      {
        expression += "0 ";
        ++vector_width;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error parsing VECTOR_WIDTH: " << e.what() << std::endl;
    }
  }
  expression += ")";

  // Write the expression to the expression_file
  expression_file << expression << std::endl;

  // Overwrite the vectorized_code_file
  vectorized_code_file << "";

  // Close the input and expression files
  inputs_file.close();
  expression_file.close();
  vectorized_code_file.close();

  // Call the vectorizer function with the computed vector width
  call_vectorizer(vector_width);

  // Re-open the vectorized_code_file in append mode and write the vector width
  std::ofstream vectorized_code_file_2("../vectorized_code.txt", std::ios::app);
  vectorized_code_file_2 << vector_width << " " << vector_width;
  vectorized_code_file_2.close();

  // Call the script to build the source code that operates on vectors
  call_script();
}

/**
 * Generates vectorized code for a given function, divided into subvectors.
 *
 * This function performs the following tasks:
 * 1. Initializes an expression printer in prefix notation to print expressions.
 * 2. Opens output files: inputs.txt and vectorized_code.txt.
 * 3. Checks if the files opened successfully.
 * 4. Processes the input terms, storing their names and types in separate strings.
 * 5. Writes the input names and types to inputs.txt.
 * 6. Processes the output terms and computes the total number of outputs (vector_full_width).
 * 7. Computes the size of each subvector based on the window parameter.
 * 8. Reads the VECTOR_WIDTH environment variable to possibly extend the vector width.
 * 9. Constructs subvectors from the output terms, ensuring each subvector is of the specified size.
 * 10. Pads subvectors with zeros if the environment variable indicates a larger vector size.
 * 11. Writes each subvector expression to expression.txt and calls the vectorizer function.
 * 12. Appends the vector_full_width and subvector size to vectorized_code.txt.
 * 13. Calls an external script to build the source code that operates on vectors.
 *
 * @param func Shared pointer to the function to be vectorized.
 * @param window The number of subvectors to divide the outputs into for vectorization.
 */
void Compiler::gen_vectorized_code(const std::shared_ptr<ir::Func> &func, int window)
{

  if (window <= 0)
  {
    std::cerr << "Window size must be greater than 0." << std::endl;
    return;
  }
  else if (window == 1)
  {
    gen_vectorized_code(func);
    return;
  }
  else
  {
    // Utility function to print expressions in prefix notation
    util::ExprPrinter expr_printer(func);
    expr_printer.make_terms_str_expr(util::ExprPrinter::Mode::prefix);

    // Initialize files to store inputs and vectorized code
    std::ofstream inputs_file("../inputs.txt");
    std::ofstream vectorized_code_file("../vectorized_code.txt");

    // Check if any of the files failed to open
    if (!inputs_file || !vectorized_code_file)
    {
      std::cerr << "Error opening one of the output files." << std::endl;
      return;
    }

    // Strings to store input names and types
    std::string input_names;
    std::string input_types;

    // Process input terms and store their names and types
    std::vector<const ir::Term *> input_terms;
    for (const auto &input_info : func->data_flow().inputs_info())
    {
      input_terms.push_back(input_info.first);
    }
    for (auto it = input_terms.rbegin(); it != input_terms.rend(); ++it)
    {
      auto input_term = *it;
      input_names += expr_printer.terms_str_exprs().at(input_term->id()) + " ";
      input_types += (input_term->type() == ir::Term::Type::cipher) ? "1 " : "0 ";
    }
    inputs_file << input_names << std::endl;
    inputs_file << input_types << std::endl;
    inputs_file.close();

    // Process output terms and compute the total number of outputs
    std::vector<const ir::Term *> output_terms;
    int vector_full_width = 0;
    for (const auto &output_info : func->data_flow().outputs_info())
    {
      output_terms.push_back(output_info.first);
      vector_full_width++;
    }

    // Compute the size of each subvector based on the window parameter
    int sub_vector_size = vector_full_width / window;
    int index = 0;
    std::string expression = "(Vec ";
    int vector_width = 0;

    // Get the vector width from the environment variable
    const char *env_var = std::getenv("VECTOR_WIDTH");
    if (env_var)
    {
      try
      {
        vector_width = std::stoi(env_var);
      }
      catch (const std::exception &e)
      {
        std::cerr << "Error parsing VECTOR_WIDTH: " << e.what() << std::endl;
      }
    }

    // Clear the content of vectorized_code_file
    vectorized_code_file << "";
    vectorized_code_file.close();

    // Generate vectorized code
    for (auto it = output_terms.rbegin(); it != output_terms.rend(); ++it)
    {
      auto output_term = *it;
      expression += expr_printer.terms_str_exprs().at(output_term->id()) + " ";
      index = (index + 1) % sub_vector_size;

      // When a subvector is complete or all outputs are consumed
      if (!index || it == output_terms.rend() - 1)
      {
        int current_vector_width = (index == 0) ? sub_vector_size : index + 1;

        // Pad the subvector with zeros if necessary
        for (int i = 0; i < vector_width - current_vector_width; ++i)
        {
          expression += " 0 ";
          current_vector_width++;
        }
        expression += " )";

        // Write the expression to expression_file and call the vectorizer
        std::ofstream expression_file("../expression.txt");
        if (!expression_file)
        {
          std::cerr << "Error opening expression file." << std::endl;
          return;
        }
        expression_file << expression;
        expression_file.close();

        call_vectorizer(current_vector_width);
        expression = "(Vec ";
      }
    }

    // Append vector_full_width and sub_vector_size to vectorized_code.txt
    std::ofstream vectorized_code_file_2("../vectorized_code.txt", std::ios::app);
    if (!vectorized_code_file_2)
    {
      std::cerr << "Error opening vectorized code file." << std::endl;
      return;
    }
    vectorized_code_file_2 << vector_full_width << " " << sub_vector_size;
    vectorized_code_file_2.close();

    // Call the script to construct the source code
    call_script();
  }
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