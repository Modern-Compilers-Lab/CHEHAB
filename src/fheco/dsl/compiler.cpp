#include "fheco/code_gen/gen_func.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/plaintext.hpp"
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
#include <sstream>
#include <unordered_map>
#include <algorithm>

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
  string_view header_name, ostream &source_os, bool log2_reduct,
  param_select::EncParams::SecurityLevel security_level)
{ 
  auto rewrite_heuristicc = trs::RewriteHeuristic::bottom_up;
  trs::TRS joined_trs{trs::Ruleset::joined_ruleset(func)};
  joined_trs.run(rewrite_heuristicc); 
  passes::cse_commut(func);
  /***********Apply Sumvec reduction ************/
  trs::TRS SumVec_reduct_trs{trs::Ruleset::SumVec_reduct_opt_ruleset(func)};
  SumVec_reduct_trs.run(trs::RewriteHeuristic::top_down);
  /***********************Rename io_file*********/
  std::string input_file_name = "fhe_io_example.txt";
  std::ifstream input_file(input_file_name);
  string updated_inputs_file_name = "fhe_io_example_adapted.txt" ;
  std::ofstream updated_input_file(updated_inputs_file_name);
  std::string line;
  if (input_file.is_open()&&updated_input_file.is_open()) {
      while (std::getline(input_file, line)) {
          updated_input_file<<line<<"\n";
      }
      updated_input_file.close();
      input_file.close();
  }
  /*********************************************/
  gen_he_code(func, header_os, header_name, source_os, 29, true);
}

void Compiler::gen_he_code(
  const std::shared_ptr<ir::Func> &func, std::ostream &header_os, std::string_view header_name, std::ostream &source_os,
  size_t rotation_keys_threshold, bool lazy_relin,
  param_select::EncParams::SecurityLevel security_level)
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
  code_gen::gen_func(func, rotation_steps_keys, header_os, header_name, source_os,security_level);
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
  std::cout<<"welcome in vectorized code generator \n";
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
  //std::cout<<"Getting expressions inttputs Infos \n";
  auto process_input_terms = [&](const ir::InputTermsInfo &inputs_info) {
    //std::cout<<"welcome \n";
    std::vector<const ir::Term *> input_terms;
    vector<string> prepared_names ={} ;
    for (const auto &input_info : inputs_info)
    {
      input_terms.push_back(input_info.first);
      string name=input_info.second.label_;
      prepared_names.push_back(name);
    }
    std::reverse(prepared_names.begin(),prepared_names.end());
    //std::cout<<"format expression \n";
    int comp =0 ;
    for (auto it = input_terms.rbegin(); it != input_terms.rend(); ++it)
    {
      auto input_term = *it;
      //input_names += expr_printer.terms_str_exprs().at(input_term->id()) + " ";
      input_names+=prepared_names[comp]+" ";
      //std::cout<<input_names<<" \n";
      input_types += (input_term->type() == ir::Term::Type::cipher) ? "1 " : "0 ";
      comp+=1;
    }
  };
  // Process input terms and write to inputs_file
  process_input_terms(func->data_flow().inputs_info());
  inputs_file << input_names << std::endl;
  inputs_file << input_types << std::endl;

  // Helper function to process output terms and return them as a vector
  //std::cout<<"Getting expressions outputs Infos \n";
  auto process_output_terms = [&](const ir::OutputTermsInfo &outputs_info , const ir::orderedOutputTermsKeys & output_keys) {
    std::vector<const ir::Term *> output_terms;
    for(const auto & output_key : output_keys){
      auto output_info = outputs_info.at(output_key); 
      unordered_set<std::string> infos = output_info.labels_ ;
      output_terms.push_back(output_key);
    }
    return output_terms;
  };

  // Process output terms
  std::vector<const ir::Term *> output_terms = process_output_terms(func->data_flow().outputs_info(),func->data_flow().output_keys());
  std::string expression = "(Vec ";
  //  for (auto it = output_terms.begin(); it != output_terms.end(); ++it)
  for (auto it = output_terms.begin(); it != output_terms.end(); ++it)
  {
    auto output_term = *it;
    expression += expr_printer.terms_str_exprs().at(output_term->id()) + " ";
    //std::cout<<expression<<" \n";
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
  /*********************************************************/
  // Call the vectorizer function with the computed vector width
  std::cout<<"Call the code vectorizer \n";
  call_vectorizer(vector_width);
  /***********************************************************/
  // Re-open the vectorized_code_file in append mode and write the vector width
  std::ofstream vectorized_code_file_2("../vectorized_code.txt", std::ios::app);
  /*************************************************
  vectorized_code_file_2 << expression << "\n";
  /*********************************************** */
  vectorized_code_file_2 << vector_width << " " << vector_width;
  vectorized_code_file_2.close();
  // Call the script to build the source code that operates on vectors
  format_vectorized_code(func);
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

  if (window < 0)
  {
    std::cerr << "Window size must be greater than 0." << std::endl;
    return;
  }
  else if (window == 0)
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
    else
    {
      vector_width = window;
    }

    // Clear the content of vectorized_code_file
    vectorized_code_file << "";
    vectorized_code_file.close();
    // Generate vectorized code
    for (auto it = output_terms.rbegin(); it != output_terms.rend(); ++it)
    {
      auto output_term = *it;
      expression += expr_printer.terms_str_exprs().at(output_term->id()) + " ";
      index = (index + 1) % window;

      // When a subvector is complete or all outputs are consumed
      if (!index || it == output_terms.rend() - 1)
      {
        int current_vector_width = (index == 0) ? window : index;
        // Pad the subvector with zeros if necessary
        for (int i = 0; i < vector_width - current_vector_width; ++i)
        {
          expression += " 0 ";
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

        call_vectorizer(vector_width);
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
    vectorized_code_file_2 << vector_full_width << " " << window;
    vectorized_code_file_2.close();

    // Call the script to construct the source code
    format_vectorized_code(func);
  }
}
/***************************************************************************************/
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
/***************************************************************************************/
/**************************************************************************************/
using namespace std ;
std::unordered_map<int, std::string> labels_map;
string new_inputs;
std::vector<string> new_inputs_labels ;
int id_counter = 0;
/**************************************************************************/
ir::OpCode Compiler::operationFromString(string operation)
{
  if (operation == "+")
    return ir::OpCode::add;
  else if (operation == "-")
    return ir::OpCode::sub;
  else if (operation == "*")
    return ir::OpCode::mul;
  else if (operation == "square")
    return ir::OpCode::square;
  else
    throw logic_error("Invalid expression");
}
/*****************************************************************/
ir::Term *Compiler::build_expression(const std::shared_ptr<ir::Func> &func, map<string, ir::Term *> map, queue<string> &tokens)
{
  while (!tokens.empty())
  {

    if (tokens.front() == "(")
    {
      bool op_is_rotation = false;
      bool op_is_SumVec = false ;
      tokens.pop();
      string operationString = tokens.front();
      ir::OpCode operation = ir::OpCode::nop;
      if (operationString == "<<"){
          op_is_rotation = true;
      }else if (operationString =="SumVec"){
          op_is_SumVec = true ; 
      }else{
          operation = operationFromString(operationString);
      }
      tokens.pop();

      string potential_step = "";

      ir::Term *operand1, *operand2 = nullptr;
      if (tokens.front() == "(")
      {
        operand1 = build_expression(func, map, tokens);
      }
      else
      {
        operand1 = map.at(tokens.front());
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        potential_step += " ";
        operand2 = build_expression(func, map, tokens);
      }
      else if (tokens.front() != ")")
      {
        if (!op_is_rotation && !op_is_SumVec)
        {
          operand2 = map.at(tokens.front());
        }
        potential_step = tokens.front();

        tokens.pop();
      }

      // Check for the closing parenthesis
      if (tokens.front() == ")")
      {
        tokens.pop();
      }

      if (potential_step.size() > 0)
      {

        if (op_is_rotation)
        {
          operation = ir::OpCode::rotate(stoi(potential_step));
          vector<ir::Term *> operands = {operand1};
          return func->insert_op_term(move(operation), move(operands));
        }
        else if (op_is_SumVec){
          operation = ir::OpCode::SumVec(stoi(potential_step));
          vector<ir::Term *> operands = {operand1};
          return func->insert_op_term(move(operation), move(operands));
        }else{
          vector<ir::Term *> operands = {operand1, operand2};
          return func->insert_op_term(move(operation), move(operands));
        }
      }
      else
      {
        vector<ir::Term *> operands = {operand1};
        if (operation == ir::OpCode::sub)
          operation = ir::OpCode::negate;
        return func->insert_op_term(move(operation), move(operands));
      }
    }
    else
    {

      return map.at(tokens.front());
    }
  }
  throw logic_error("Invalid expression");
}
/*****************************************************************/
queue<string>split(const string &s)
{
  queue<std::string> tokens;
  stringstream ss(s);
  std::string token;
  while (getline(ss, token, ' '))
  {
    tokens.push(token);
  }
  return tokens;
}
/**************************************************************************/
void replace_all(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Move past the replacement
    }
}
/************************************************************************/
std::vector<std::string> process_vectorized_code(const string& content) {
    std::string cleaned_content = content;
    replace_all(cleaned_content, "(", "( ");
    replace_all(cleaned_content, ")", " )");
    replace_all(cleaned_content, "\n", " ");
    /********************************/
    istringstream iss(cleaned_content);
    vector<string> tokens;
    string token;

    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}
/***********************************************************************/
bool is_literal(const std::string& token) {
    return std::none_of(token.begin(), token.end(), [](unsigned char c) { return std::isalpha(c); });
}
/**********************************************************************/
std::pair<std::string, int> process(
    const std::vector<std::string>& tokens,
    int index,
    std::unordered_map<std::string, std::string>& dictionary,
    const std::vector<std::string>& inputs,
    const std::vector<std::string>& inputs_types,
    int slot_count,
    int sub_vector_size,
    string& new_expression
) {
    while (index < tokens.size()) {
        if (tokens[index] == "(") {
            index++;
            if (tokens[index] == "Vec"){
                std::string string_vector = "Vec ";
                int nested_level = 0;
                index++;
                while (nested_level >= 0) {
                    string_vector += tokens[index] + " ";
                    nested_level += (tokens[index] == "(") ? 1 : (tokens[index] == ")") ? -1 : 0;
                    index++;
                }
                /*
                String_vector :Vec in_74 in_60 ( + in_43 ( + in_23 in_33 ) ) )
                */
                string_vector = string_vector.substr(0, string_vector.size() - 2);  // Strip trailing space and closing parenthesis
                if (dictionary.find(string_vector) == dictionary.end()) {
                    vector<string> elements;
                    istringstream iss(string_vector.substr(4));  // Remove "Vec " from vector string
                    string element;
                    string new_input = "";
                    bool all_literals = true;
                    int vector_index=0;
                    // Start treating a vector content
                    /*******************************************************************/
                    /*******************************************************************/
                    // ( + ( * A[58][] B[][0] ) ( << ( * A[58][] B[][0] ) 1 ) )
                    //std::cout<<"\n===>Start Vector treatment :\n";
                    bool is_vector_contain_sub_ops = false ;
                    string sub_vector_expression = "";
                    int nb_sub_elements = 0 ;
                    while (iss >> element) {
                        if (element=="("){
                          nb_sub_elements+=1;
                          sub_vector_expression+=" ( +";
                          /****/sub_vector_expression+=" (" ;
                          //string sub_expression="";
                          int sub_nested_level=0 ;
                          bool end = false ;
                          while (iss >> element&&sub_nested_level>=0){
                              if (element=="+"|| element=="*" || element=="-"){
                                 /*****/sub_vector_expression+=" "+element ;
                              }else{
                                if(element!=")"&&element!="("){
                                  string new_element =" ";
                                  //std::cout<<"Fomating the new element :\n";
                                  for(int i =0; i<sub_vector_size;i++){
                                    if(i==vector_index){
                                      new_element+=element+" "; 
                                    }else{
                                      new_element+="0 ";
                                    }
                                  }
                                  /*******Add the label of the new elements**/
                                  string temp_str = "Vec"+new_element;
                                  if (dictionary.find(temp_str) == dictionary.end()) {
                                      int number_of_sub_vectors = slot_count / sub_vector_size;
                                      string res = "";
                                      for(int i =0 ; i<number_of_sub_vectors ; i++){
                                          res+=" " + new_element ;
                                      }
                                      res = new_element ;
                                      new_element = new_element.substr(1);  // Remove leading space
                                      string label;
                                      label = "c" + std::to_string(id_counter);
                                      new_element = "1 1 " + new_element;
                                      new_inputs_labels.push_back(label);
                                      labels_map[id_counter] = label;
                                      new_inputs += label+" "+new_element + "\n";
                                      id_counter++;
                                      dictionary[temp_str] = label;
                                      /*****/sub_vector_expression+=" "+label ;
                                  }
                                  /****this input element Exist in the dictionarr */
                                  else{
                                      sub_vector_expression+=" "+dictionary[temp_str] ;
                                  }
                                }else{    
                                  sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                                  /*****/sub_vector_expression+=" "+element ;
                                }
                              }
                          }
                          iss.seekg(-element.length(), std::ios_base::cur);
                          new_input+="0 ";
                        }
                        /*********************************************************/
                        else{
                          if (!is_literal(element)) {
                              new_input += element + " ";
                              if (inputs_types[std::distance(inputs.begin(), std::find(inputs.begin(), inputs.end(), element))] == "1") {
                                  all_literals = false;
                              }
                          } else {
                            new_input += element+" ";
                          }
                        }
                        vector_index+=1;
                        //std::cout<<"current_vector_shape :"<<new_input<<" , Current_pos :"<<vector_index<<"\n";
                    }
                    //std::cout<<"End vector treatment \n";
                    /**************************************************************************/
                    /**************************************************************************/
                    int number_of_sub_vectors = slot_count / sub_vector_size;
                    string res = "";
                    for(int i =0 ; i<number_of_sub_vectors ; i++){
                        res+=" " + new_input ;
                    }
                    new_input = res ;
                    new_input = new_input.substr(1);  // Remove leading space
                    string label, label_type;
                    if (all_literals) {
                        label = "p" + std::to_string(id_counter);
                        new_input = "0 1 " + new_input;
                    } else {
                        label = "c" + std::to_string(id_counter);
                        new_input = "1 1 " + new_input;
                    }
                    new_inputs_labels.push_back(label);
                    labels_map[id_counter] = label;
                    new_inputs += label+" "+new_input + "\n";
                    id_counter++;
                    dictionary[string_vector] = label;
                    if(nb_sub_elements>0){
                      sub_vector_expression+=" "+label;
                      for(int i=0;i<nb_sub_elements;i++){
                        sub_vector_expression+=" )";
                      }
                      /*******/new_expression+=sub_vector_expression;
                    }else{
                       /******/new_expression+=" "+label ;
                    }
                }else{
                    new_expression+=" "+dictionary[string_vector] ;
                }
                return {dictionary[string_vector], index};
            }
            /******/new_expression+=" (";
            std::string operation = tokens[index];
            std::string op = (operation == "VecAdd") ? "+" : (operation == "VecMinus") ? "-" : (operation == "VecMul") ? "*" : "<<";
            /*****/new_expression+=" "+op ;
            index++;
            auto [operand_1, new_index] = process(tokens, index, dictionary, inputs, inputs_types, slot_count, sub_vector_size,new_expression);
            index = new_index;
            /**************************/
            if (tokens[index] != ")") {
                std::string operand_2;
                if (tokens[index] == "(") {
                    std::tie(operand_2, index) = process(tokens, index, dictionary, inputs, inputs_types, slot_count, sub_vector_size,new_expression);
                } else {
                    operand_2 = tokens[index];
                    new_expression+=" "+operand_2;
                    index++;
                }
                /******/new_expression+=" )";
                std::string op = (operation == "VecAdd") ? "+" : (operation == "VecMinus") ? "-" : (operation == "VecMul") ? "*" : "<<";
                std::string label = "c" + std::to_string(id_counter);
                //std::cout<<"generated label :"<<label<<" \n";
                labels_map[id_counter] = label;
                id_counter++;
                index++;
                return {label, index};
            } else {
                /******/new_expression+=" )";
                index++;
                std::string label = "c" + std::to_string(id_counter);
                labels_map[id_counter] = label;
                id_counter++;
                return {label, index};
            }
        }
    }
    return {"", index};
}
/**********************************************************************/
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }
    return substrings;
}
/************************************************************************/
void update_io_file(const string& updated_inputs,const vector<string> updated_outputs, int new_slot_count){
   std::string inputs_file_name = "fhe_io_example.txt";
   std::ifstream input_file(inputs_file_name);
   std::string line ;
   std::vector<string> lines ;
   //std::cout<<" read fhe_io example file \n";
   if (input_file.is_open()) {
        //std::cout<<"read file content \n";
        while (std::getline(input_file, line)) {
            //std::cout<<line<<" \n";
            lines.push_back(line);
        }
        input_file.close();
    } else {
        std::cerr << "Unable to open file: " << inputs_file_name << std::endl;
    }
    /***************************************************************************/
    vector<string> separated_updated_inputs = split_string(updated_inputs, '\n');
    std::unordered_map<string,string> plaintexts;
    std::unordered_map<string,string> ciphertexts;
    std::vector<string> old_header = split_string(lines[0], ' '); 
    if(old_header.size()!=3){
        throw invalid_argument("malformatted io_file header\n");
    }
    int old_slot_count = stoi(old_header[0]);
    int old_nb_inputs = stoi(old_header[1]);
    for(int i=1 ; i< old_nb_inputs+1 ; i++){
        vector<std::string> tokens = split_string(lines[i], ' ');
        if(tokens.size()<old_slot_count+3){
            throw invalid_argument("malformated input file");
        }
        string input_name = tokens[0];
        int type = stoi(tokens[1]);
        string value = "";
        for(int i=3 ; i<tokens.size() ; i++){
            value+=tokens[i]+" ";
        }
        // input type is ciphertext
        if(type==1){
            ciphertexts[input_name]=value ;
        }
        // input type is plaintext
        else{
            plaintexts[input_name]=value ;
        }
    }
    /**************************************************/
    string updated_inputs_file_name = "fhe_io_example_adapted.txt" ;
    std::ofstream updated_input_file(updated_inputs_file_name);
    /**********************************************************/
    string new_header = std::to_string(new_slot_count)+" "+std::to_string(separated_updated_inputs.size())+" "+std::to_string(updated_outputs.size())+"\n";
    updated_input_file << new_header;
    string updated_input ="" ;
    for(int i = 0 ;i<separated_updated_inputs.size();i++){
        updated_input ="" ;
        vector<std::string> tokens = split_string(separated_updated_inputs[i], ' ');
        if(tokens.size()>3){
            updated_input=tokens[0]+" "+tokens[1]+" "+tokens[2]+" ";
            int type = stoi(tokens[1]);
            for(int i =3; i<tokens.size() ; i++){
                string key = tokens[i];
                if(!is_literal(key)){
                    string value =""; 
                    if (ciphertexts.find(key) != ciphertexts.end()) {
                        updated_input+=ciphertexts[key];  // Access the value corresponding to the key
                    } else {
                        throw invalid_argument("key : "+key+" Not found in ciphertxts map \n");
                    }
                }else{
                    updated_input+=key+" ";
                }
            }
            updated_input+="\n";
            //std::cout<<updated_input ;
            updated_input_file << updated_input;
        }else{
            throw invalid_argument("malfomated updated inputs\n");
        }
    }
    for(int j=0;j<updated_outputs.size();j++){
        updated_input=updated_outputs[j]+" 1";
        for(int i =0;i<new_slot_count;i++){
            updated_input+=" 0";
        }
        updated_input+="\n";
        updated_input_file << updated_input;
    }
    updated_input_file.close();
}
/************************************************************************/
/************************************************************************/
void Compiler::format_vectorized_code(const std::shared_ptr<ir::Func> &func)
{
    new_inputs = "";
    new_inputs_labels = {};
    std::string inputs_file = "../inputs.txt";
    std::ifstream input_file(inputs_file);
    std::string inputs_line, input_types_line;
    if (input_file.is_open()) {
        std::getline(input_file, inputs_line);
        std::getline(input_file, input_types_line);
        input_file.close();
    }
    std::vector<std::string> inputs, inputs_types;
    std::istringstream iss_inputs(inputs_line), iss_types(input_types_line);
    std::string token;
    while (iss_inputs >> token) inputs.push_back(token);
    while (iss_types >> token) inputs_types.push_back(token);
    /********************************************************************/
    std::string vectorized_file = "../vectorized_code.txt";
    //std::string vectorized_file = "../expression.txt";
    /*********************************************************************/
    std::ifstream vec_file(vectorized_file);
    std::vector<std::string> expressions;
    std::string expression;
    if (vec_file.is_open()) {
        while (std::getline(vec_file, expression)) {
            //std::cout<<expression<<" \n";
            expressions.push_back(expression);
        }
        vec_file.close();
    }
    int slot_count = std::stoi(expressions.back().substr(0, expressions.back().find(' ')));
    int sub_vector_size = std::stoi(expressions.back().substr(expressions.back().find(' ') + 1));
    //std::cout<<"slot_count : "<<slot_count<<" \n";
    //std::cout<<"sub_vector_size : "<<sub_vector_size<<" \n";
    std::vector<std::string> outputs;
    string simplified_expression = "";
    //std::cout<<"Start Processing of vectorized code :\n" ;
    for (const auto& expr : expressions) {
        if (&expr == &expressions.back()) break;
        auto tokens = process_vectorized_code(expr);
        std::unordered_map<std::string, std::string> dictionary = {};
        process(tokens,0,dictionary, inputs, inputs_types, slot_count, sub_vector_size,simplified_expression);
        simplified_expression+="\n";
        outputs.push_back(labels_map[id_counter - 1]);
    }
    simplified_expression = simplified_expression.substr(1); 
    //std::cout<<"Updated IR : "<<simplified_expression<<" \n";
    /*****************************************************************/
    /*****************************************************************/
    update_io_file(new_inputs,outputs,sub_vector_size);
    std::cout<<"==>IO file updated succefully \n";
    /*****************************************************************/
    /*******Convert simplified_vectorized IR  *************************/
    func->reset_data_flow();
    //std::cout<<"Number of function inputs : "<<func->data_flow().inputs_info().size()<<"\n";
    func->set_slot_count(sub_vector_size);
    util::ExprPrinter pr(func);
    pr.make_terms_str_expr(util::ExprPrinter::Mode::prefix);
    map<string, ir::Term *> myMap;
    ////**********Storing input infos *********************
    vector<string> new_input_infos = split_string(new_inputs,'\n');
    //std::cout<<"Creating the new inputs \n";
    for(auto new_input_info : new_input_infos){
      //std::cout<<new_input_info<<" \n";
      vector<std::string> tokens = split_string(new_input_info, ' ');
      string label = tokens[0]; 
      int type = stoi(tokens[1]);
      if(type==1){
          //std::cout<<"here c\n";
          Ciphertext cipher(label);
          func->init_input(cipher,move(label));
      }else{
          //std::cout<<"here1 \n";
          Plaintext plain(label);
          func->init_input(plain,move(label));
      }
    }
    //std::cout<<"Number of function inputs : "<<func->data_flow().inputs_info().size()<<"\n";
    for(auto new_output_label : outputs){
      //std::cout<<"Add new output :"<<new_output_label<<" \n";
      Ciphertext cipher(new_output_label);
      func->set_output(cipher,move(new_output_label));
    }
    //std::cout<<"Store input_elements in myMap \n";
    for (auto input_info : func->data_flow().inputs_info())
    {
      //std::cout<<"insert term in myMap with label :"<<input_info.second.label_<<"\n";
      ir::Term *temp = const_cast<ir::Term *>(input_info.first);
      myMap[input_info.second.label_] = temp;
    }
    vector<const ir::Term *> output_terms;
    
    //std::cout<<"Store output_elements in myMap \n";
    for (auto output_info : func->data_flow().outputs_info())
    {
      output_terms.push_back(output_info.first);
    }
    ////////////////////////
    std::reverse(output_terms.begin(), output_terms.end());
    std::string new_term_str ;
    stringstream ss(simplified_expression);
    int index=0;
    //std::cout<<"Number of function inputs : "<<func->data_flow().inputs_info().size()<<"\n";
    //std::cout<<"Start expression traitment ====> \n";
    while (getline(ss,new_term_str)) {
          std::cout<<new_term_str<<" \n";
          if (!new_term_str.empty()) {  // Ensure that we do not push empty tokens
              //std::cout<<"here \n";
              auto tokens = split(new_term_str);
              //std::cout<<"here1 \n";
              auto new_term = build_expression(func, myMap, tokens);
              //std::cout<<"here2 \n";
              auto old_term = const_cast<ir::Term *>(output_terms[index]);
              func->replace_term_with(old_term, new_term);
              //std::cout<<"here3 \n";
              index+=1;
          }
    }
    //std::cout<<"Number of function inputs : "<<func->data_flow().inputs_info().size()<<"\n";
    /*******************************************************************/
    /*******************************************************************/
    /*const char *command = "python3 ../script.py ";
    int result = system(command);
    if (result != 0)
    {
      // The executable did not run successfully
      std::cout << "Failed to call the script " << std::endl;
    }*/
}
} // namespace fheco
