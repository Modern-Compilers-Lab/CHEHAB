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

bool Compiler::automatic_enc_params_enabled_ = false; // Or set to true if desired


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
  code_gen::gen_func(func, rotation_steps_keys, header_os, header_name, source_os,security_level,auto_enc_params_selection_enabled());
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
/************************************************************************/
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
string constant_folding(queue<string> &tokens)
{
  //std::cout<<"welcome in constant folding\n";
  while (!tokens.empty())
  {
    //std::cout<<"hereee :"<<tokens.front()<<"\n";
    if (tokens.front() == "(")
    {
      //std::cout<<"here\n";
      tokens.pop();
      string operationString = tokens.front();
      tokens.pop();
      string potential_step = "";
      string operand1="" ,operand2="";
      if (tokens.front() == "(")
      {
        operand1 = constant_folding(tokens);
      }
      else
      {
        //std::cout<<"get op1 \n";
        operand1 = tokens.front();
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        operand2 = constant_folding(tokens);
        potential_step += " ";

      }
      else if (tokens.front() != ")")
      {
        //std::cout<<"get op2 \n";
        operand2 = tokens.front();
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
        //std::cout<<operationString<<" "<<operand1<<" "<<operand2<<" \n";
        bool is_op1_litteral = is_literal(operand1);
        bool is_op2_litteral = is_literal(operand2);
        //std::cout<<type_op1<<" "<<type_op2<<" \n";
        if(is_op1_litteral&&is_op2_litteral){
          int op1 = 0;
          int op2 = 0;
          try{
            op1 = stoi(operand1);
            op2 = stoi(operand2); 
            int res = 0 ;
            if(operationString=="+"){
              res = op1+op2 ;
            }else if(operationString=="-"){
              res = op1-op2 ;
            }else if(operationString=="*"){
              res = op1*op2 ;
            }
            string res_op = std::to_string(res);
            return res_op ;
          }catch(exception e){
            throw invalid_argument("value :"+operand1+" or "+operand2+" cant be converted to int");
          }
        }else if(is_op1_litteral){
            int op1 = stoi(operand1);
            int res = 0;
            if(op1==0){
              if(operationString=="+"){
                return operand2;
              }else if(operationString=="-"){
                return "( "+operationString+" "+operand2+" )";
              }else if(operationString=="*"){
                return std::to_string(res);
              }
            }else if (op1==1){
              if(operationString=="*"){
                return operand2;
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else if (op1==-1){
             if(operationString=="*"){
                return "( - "+operand2+" )";
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else{
               return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else if (is_op2_litteral){
            //std::cout<<"welcome \n";
            int op2 = stoi(operand2);
            int res = 0;
            if(op2==0){
              if(operationString=="+"){
                return operand1;
              }else if(operationString=="-"){
                return "( "+operationString+" "+operand1+" )";
              }else if(operationString=="*"){
                return std::to_string(res);
              }
            }else if (op2==1){
              if(operationString=="*"){
                return operand1;
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else if (op2==-1){
             if(operationString=="*"){
                return "( - "+operand1+" )";
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }
            else{
               return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else{
            return "( "+operationString+" "+operand1+" "+operand2+" )" ;
        }
      }else{
        return "( "+operationString+" "+operand1+" )" ;
      }
    }
    else
    {
      return tokens.front();
    }
  }
  throw logic_error("Invalid expression");
}
/**********************************************************************/
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
  if (!inputs_file || !vectorized_code_file || !expression_file)
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
    string temp_elem = expr_printer.terms_str_exprs().at(output_term->id()) ;
    auto tokens = split(temp_elem);
    temp_elem=constant_folding(tokens);
    expression += temp_elem + " ";
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
    vector<string> prepared_names ={} ;
    for (const auto &input_info : func->data_flow().inputs_info())
    {
      input_terms.push_back(input_info.first);
      string name=input_info.second.label_;
      prepared_names.push_back(name);
    }
    std::reverse(prepared_names.begin(),prepared_names.end());
    int comp = 0;
    for (auto it = input_terms.rbegin(); it != input_terms.rend(); ++it)
    {
      auto input_term = *it;
      input_names+=prepared_names[comp]+" ";
      input_types += (input_term->type() == ir::Term::Type::cipher) ? "1 " : "0 ";
      comp+=1;
    }
    inputs_file << input_names << std::endl;
    inputs_file << input_types << std::endl;
    inputs_file.close();
    // Process output terms and compute the total number of outputs
    int vector_full_width = 0;
    
    auto process_output_terms = [&](const ir::OutputTermsInfo &outputs_info , const ir::orderedOutputTermsKeys & output_keys) {
      std::vector<const ir::Term *> output_terms;
      for(const auto & output_key : output_keys){
        vector_full_width++;
        auto output_info = outputs_info.at(output_key); 
        unordered_set<std::string> infos = output_info.labels_ ;
        output_terms.push_back(output_key);
      }
      return output_terms;
    }; 
    // Process output terms
    std::vector<const ir::Term *> output_terms = process_output_terms(func->data_flow().outputs_info(),func->data_flow().output_keys());
    if(vector_full_width<window){
      std::cout<<"\nresult vector width smaller than window size ==> windows will be considered=0(deactivated)\n";
      gen_vectorized_code(func);
      return;
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
    for (auto it = output_terms.begin(); it != output_terms.end(); ++it)
    {
      auto output_term = *it;
      expression += expr_printer.terms_str_exprs().at(output_term->id()) + " ";
      index = (index + 1) % window;
      // When a subvector is complete or all outputs are consumed
      if (!index || it == output_terms.end() - 1)
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
/***************************************************************************/
using namespace std ;
std::unordered_map<int, std::string> labels_map;
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
/*************************************************************************/
ir::Term *Compiler::build_expression(const std::shared_ptr<ir::Func> &func, map<string, ir::Term *> map, queue<string> &tokens)
{
  //std::cout<<"==========> welcome in build expression \n";
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
      //std::cout<<"operation :"<<operation<<"||\n";
      tokens.pop();

      string potential_step = "";

      ir::Term *operand1, *operand2 = nullptr;
      if (tokens.front() == "(")
      {
        //std::cout<<"build opearnd 1 \n";
        operand1 = build_expression(func, map, tokens);
      }
      else
      {
        //std::cout<<"get opearnd 1 from token"<<tokens.front()<<"||\n";
        operand1 = map.at(tokens.front());
        tokens.pop();
      }
      std::queue<string> temp = tokens;
      // Iterate through the copy and print each element
      if (tokens.front() == "(")
      {
        //std::cout<<"build operand2 \n";
        potential_step += " ";
        operand2 = build_expression(func, map, tokens);
      }
      else if (tokens.front() != ")")
      {
        //std::cout<<"get opearnd 2 from token"<<tokens.front()<<"||\n";
        if (!op_is_rotation && !op_is_SumVec)
        {
          operand2 = map.at(tokens.front());
        }
        potential_step = tokens.front();
        //std::cout<<"potential_step op2 :"<<potential_step<<" \n";
        tokens.pop();
      }

      // Check for the closing parenthesis
      if (tokens.front() == ")")
      {
        //std::cout<<"pop clsoing prenthesis \n";
        tokens.pop();
      }
      //std::cout<<"potential_step size :"<<potential_step.size()<<" \n";
      if (potential_step.size() > 0)
      {
        //std::cout<<"insert binary operation :"<<operationString<<"||\n";
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
          // we need to add addditional treatments if they both plaintexts 
          // we need to evaluate they and insterm the new resulted term 
          return func->insert_op_term(move(operation), move(operands));
        }
      }
      else
      {
        //std::cout<<"operation is negation \n";
        vector<ir::Term *> operands = {operand1};
        if (operation == ir::OpCode::sub)
          operation = ir::OpCode::negate;
        return func->insert_op_term(move(operation), move(operands));
      }
    }
    else
    {
      //std::cout<<"return token :"<<tokens.front()<<"||\n";
      return map.at(tokens.front());
    }
  }
  throw logic_error("Invalid expression");
}
/**********************************************************************/
bool verify_all_vec_elems_eq0(const vector<string>& elems){
  bool all_vec_elems_eq0 = true ;
  for(auto elem : elems){
    if(elem!="0"){
      all_vec_elems_eq0=false;
    }
  }
  return all_vec_elems_eq0 ;
}
/***********************************************************************/
void decompose_vector_op(const vector<string>& vector_elements, vector<string>& vec_ops1 , vector<string>& vec_ops2){
   vector<string> elems = {};
   for (auto elem : vector_elements) {
        vector<string> elems;
        // Check for a "0" element
        if (elem == "0") {
            vec_ops1.emplace_back(elem);
            vec_ops2.emplace_back(elem);
            continue;
        }
        // Remove the first "( + " and last " )"
        elem = elem.substr(3, elem.size() - 5);
        // Stream processing
        istringstream iss(elem);
        string token, nested_expr;
        int nested_level = 0;
        while (iss >> token) {
            if (token == "(") {
                // Start a new nested expression
                nested_expr.clear();
                nested_expr += token;
                nested_level = 1;
                // Collect tokens until the nested level returns to zero
                while (nested_level > 0 && iss >> token) {
                    nested_expr += " " + token;
                    if (token == "(") ++nested_level;
                    else if (token == ")") --nested_level;
                }
                elems.emplace_back(nested_expr);
            } else {
                // Simple operand
                elems.emplace_back(token);
            }
        }
        if (elems.size() >= 2) {
            vec_ops1.emplace_back(elems[0]);
            vec_ops2.emplace_back(elems[1]);
        }else if(elems.size() == 1){
            vec_ops1.emplace_back(elems[0]);
        }
    }
    // Debug output to check results
    /*cout << "vec_ops1: ";
    for (const auto& op : vec_ops1) cout << op << " --- ";
    cout << "\nvec_ops2: ";
    for (const auto& op : vec_ops2) cout << op << " --- ";*/
}
/***********************************************************************/
bool isSingleOperandExpression(const std::string& expression) {
    // Extract the content inside the outer parentheses
    std::string content = expression.substr(4, expression.size()); // Removes "( - " and ")"
    int depth = 0; // Tracks the current depth of parentheses
    int comp = 0 ;
    istringstream iss(content);
    string token;
    vector<string> tokens = {};
    while (iss >> token) {
      tokens.push_back(token);
    }
    for (auto c : tokens) {
        if (c == "(") {
            depth++; // Entering a nested expression
        } else if (c == ")") {
            depth--; // Exiting a nested expression
        }
        if(depth == 0){
          break;
        }
        comp++ ;
    }
    bool is_unary = false ;
    if(tokens[comp+1]==")")
      is_unary = true ;
    return is_unary ;
}
/***********************************************************************/
string process_composed_vectors(const vector<string>& vector_elements,
    std::unordered_map<std::string, std::string>& dictionary,
    std::unordered_map<std::string, std::string>& inputs_entries,
    const std::vector<std::string>& inputs,
    const std::vector<std::string>& inputs_types,
    int slot_count,
    int sub_vector_size)
{
  if(!verify_all_vec_elems_eq0(vector_elements)){
    vector<string> simple_elements = {} ;
    vector<string> composed_elements = {} ;
    for(auto elem : vector_elements){
      //std::cout<<"===========> : First element ==>"<<elem.at(0)<<"\n";
      if(elem.at(0)=='('){
        composed_elements.push_back(elem);
        simple_elements.push_back("0");
      }else{
        composed_elements.push_back("0");
        simple_elements.push_back(elem);
      }
    }
    /*std::cout<<"composed elements \n";
    for(auto val : composed_elements){
      std::cout<<val<<" --- ";
    }
    std::cout<<"\n*******************************************\n";
    std::cout<<"Simple elements \n";
    for(auto val : simple_elements){
      std::cout<<val<<" --- ";
    }
    std::cout<<"\n";*/
    bool all_simple_elements_eq_0 = verify_all_vec_elems_eq0(simple_elements);
    bool all_composed_elements_eq_0 = verify_all_vec_elems_eq0(composed_elements);
    //std::cout<<"Next \n";
    vector<string> addition_elements = {} ;
    vector<string> substraction_elements = {} ;
    vector<string> multiplication_elements = {} ;
    vector<string> negation_elements = {} ;
    if(!all_simple_elements_eq_0&&!all_composed_elements_eq_0){
      return "( + "+process_composed_vectors(simple_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(composed_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
    }else if(!all_composed_elements_eq_0){
      // declare simple_elements as a new ciphertext and store it 
      // indicate that they are associated with composed elements by an addition 
      // cout<<"divide composed_elements vector on three vectors each one containing\n";
      //cout<<" ===> Treat composed vectors\n";
      for(const auto elem : composed_elements){
        if(elem=="0"){
          addition_elements.push_back("0");
          substraction_elements.push_back("0");
          multiplication_elements.push_back("0");
          negation_elements.push_back("0");
        }else{
          if(elem.at(2)=='+'){
            addition_elements.push_back(elem);
            substraction_elements.push_back("0");
            multiplication_elements.push_back("0");
            negation_elements.push_back("0");
          }else if(elem.at(2)=='-'){
            std::cout<<elem<<"||"<<isSingleOperandExpression(elem)<<"\n";
            if(isSingleOperandExpression(elem)){
              addition_elements.push_back("0");
              substraction_elements.push_back("0");
              multiplication_elements.push_back("0");
              negation_elements.push_back(elem);
            }else{
              addition_elements.push_back("0");
              substraction_elements.push_back(elem);
              multiplication_elements.push_back("0");
              negation_elements.push_back("0");
            }
          }else if(elem.at(2)=='*'){
            addition_elements.push_back("0");
            substraction_elements.push_back("0");
            multiplication_elements.push_back(elem);
            negation_elements.push_back("0");
          }
        }
      }
      /*std::cout<<"Negation elements : \n";
      for(auto val : negation_elements)
          std::cout<<val<<" ";
      std::cout<<"\n";
      std::cout<<"Addition elements : \n";
      for(auto val : addition_elements)
          std::cout<<val<<" ";
      std::cout<<"\n";
      std::cout<<"substraction_elements : \n";
      for(auto val : substraction_elements)
          std::cout<<val<<" ";
      std::cout<<"\n";
      std::cout<<"multiplication_elements : \n";
      for(auto val : multiplication_elements)
          std::cout<<val<<" ";
      std::cout<<"\n";
      std::cout<<"Processing composed vectors done !\n";*/
      vector<string> vec_ops1 ={} ;
      vector<string> vec_ops2 ={} ;
      if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" ( + "+process_composed_vectors(substraction_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(multiplication_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" ) )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)){
         return "( + "+process_composed_vectors(addition_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(substraction_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(substraction_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(multiplication_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(multiplication_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)){
        decompose_vector_op(addition_elements, vec_ops1 ,vec_ops2);
        return "( + "+process_composed_vectors(vec_ops1,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(vec_ops2,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(substraction_elements)){
        decompose_vector_op(substraction_elements, vec_ops1 ,vec_ops2);
        return "( - "+process_composed_vectors(vec_ops1,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(vec_ops2,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(multiplication_elements)){
        decompose_vector_op(multiplication_elements, vec_ops1 ,vec_ops2);
        return "( * "+process_composed_vectors(vec_ops1,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" "+process_composed_vectors(vec_ops2,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }else if(!verify_all_vec_elems_eq0(negation_elements)){
        //std::cout<<"==> Treat Negation case \n";
        decompose_vector_op(negation_elements, vec_ops1 ,vec_ops2);
        /*for(auto val : vec_ops1)
          std::cout<<val<<" ";
        std::cout<<"\n";*/
        return "( - "+process_composed_vectors(vec_ops1,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size)+" )";
      }
    }else if(!all_simple_elements_eq_0){
      //std::cout<<"==> Traet simple elements \n";
      string new_element = ""; 
      bool is_literal_val = true ;
      //std::cout<<"=> Firts \n";
      for(auto val :simple_elements){
        if(!is_literal(val)){
          if (inputs_types[std::distance(inputs.begin(), std::find(inputs.begin(), inputs.end(), val))] == "1") {
            is_literal_val = false;
          }
        }
        new_element+=val+" ";
      }
      string string_vector = "Vec "+new_element.substr(0, new_element.size() - 1);// strip trailing space
      //std::cout<<"=> Seconds "<<string_vector<<"\n";
      if (dictionary.find(string_vector) == dictionary.end()) {
        string res = "";
        for(int i = 0 ; i<slot_count ; i++){
          res+=simple_elements[i%sub_vector_size]+" ";
        }
        new_element = res.substr(0, res.size() - 1); 
        string label;
        if(is_literal_val){
          label = "p" + std::to_string(id_counter);
          new_element = "0 1 " + new_element;
          //std::cout<<label<<" : "<<new_element<<" \n";
        }else{
          label = "c" + std::to_string(id_counter);
          new_element = "1 1 " + new_element;
          //std::cout<<label<<" : "<<new_element<<" \n";
        }
        //std::cout<<"=> new_element "<<new_element<<"\n";
        labels_map[id_counter] = label;
        inputs_entries[label]=new_element;
        id_counter++ ;
        dictionary[string_vector] = label;
        return label ;
      }else{
        return dictionary[string_vector];
      }
      //std::cout<<"=> treating simple element done \n";
    }
  }
  else{
    string new_element = ""; 
    for(auto val : vector_elements){
      new_element+=val+" ";
    }
    string string_vector = "Vec "+new_element.substr(0, new_element.size() - 1);// strip trailing space
    if (dictionary.find(string_vector) == dictionary.end()) {
      string res = "";
      for(int i = 0 ; i<slot_count ; i++){
        res+=vector_elements[i%sub_vector_size]+" ";
      }
      new_element = res.substr(0, res.size() - 1); 
      string label;
      label = "p" + std::to_string(id_counter);
      new_element = "0 1 " + new_element;
      labels_map[id_counter] = label;
      inputs_entries[label]=new_element;
      id_counter++ ;
      dictionary[string_vector] = label;
      return label ;
    }else{
      return dictionary[string_vector];
    }
  }
}
/**********************************************************************/
std::pair<std::string, int> process(
    const std::vector<std::string>& tokens,
    int index,
    std::unordered_map<std::string, std::string>& dictionary,
    std::unordered_map<std::string, std::string>& inputs_entries,
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
                std::string vector_string = "Vec ";
                int nested_level = 0;
                index++;
                while (nested_level >= 0) {
                    vector_string += tokens[index] + " ";
                    nested_level += (tokens[index] == "(") ? 1 : (tokens[index] == ")") ? -1 : 0;
                    index++;
                }
                vector_string = vector_string.substr(0, vector_string.size() - 2);
                istringstream iss(vector_string.substr(4));
                vector<string> vector_elements = {};
                string element="";
                while (iss >> element) {
                  string vector_string_element="";
                  if (element=="("){
                    vector_string_element+=" (" ;
                    int sub_nested_level=0 ;
                    while (iss >> element&&sub_nested_level>=0){
                        if(element!=")"&&element!="("){
                          vector_string_element+=" "+element; 
                        }else{
                            sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                            vector_string_element+=" "+element ;
                        }
                    }
                    iss.seekg(-element.length(), std::ios_base::cur);
                    vector_elements.push_back(vector_string_element.substr(1,vector_string_element.size()));
                  }else{
                    vector_elements.push_back(element);
                  }     
                }
                vector<string> updated_vector_elements = {};
                bool if_all_vector_elems_eq0 = true ;
                //std::cout<<"Process composed vector : \n";
                for(auto elem : vector_elements){
                  auto tokens = split(elem);
                  string updated_elem = constant_folding(tokens);
                  //std::cout<<"updated element :"<<updated_elem<<"\n";
                  updated_vector_elements.push_back(updated_elem);
                }
                string result_expr = process_composed_vectors(updated_vector_elements,dictionary,inputs_entries,inputs,inputs_types,slot_count,sub_vector_size);
                //std::cout<<"Geat \n";
                //std::cout<<"Proposing composed_vector done, res_expression :"<<result_expr<<" \n";
                /******/new_expression+=" "+result_expr;
                if(result_expr.substr(0,1)=="("){
                  std::string label = "c" + std::to_string(id_counter);
                  labels_map[id_counter] = label;
                  id_counter++;
                  return {label, index};
                }else{
                  return {dictionary[vector_string], index};
                }
            }
            /******/new_expression+=" (";
            std::string operation = tokens[index];
            std::string op = (operation == "VecAdd") ? "+" : (operation == "VecMinus") ? "-" : (operation == "VecNeg") ? "-": (operation == "VecMul") ? "*" : "<<";
            /*****/new_expression+=" "+op ;
            index++;
            auto [operand_1, new_index] = process(tokens, index, dictionary, inputs_entries,inputs,inputs_types, slot_count, sub_vector_size,new_expression);
            index = new_index;
            if (tokens[index] != ")") {
                std::string operand_2;
                if (tokens[index] == "(") {
                    std::tie(operand_2, index) = process(tokens, index, dictionary, inputs_entries,inputs,inputs_types, slot_count, sub_vector_size,new_expression);
                } else {
                    operand_2 = tokens[index];
                    new_expression+=" "+operand_2;
                    index++;
                }
                /******/new_expression+=" )";
                std::string label = "c" + std::to_string(id_counter);
                labels_map[id_counter] = label;
                id_counter++;
                index++;
                return {label, index};
            }else{
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
std::vector<std::string> split_string(const std::string& str, char delimiter){
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }
    return substrings;
}
/************************************************************************/
std::vector<int> split_string_ints(const std::string& str, char delimiter){
    std::vector<int> composingValues;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
      try{
          composingValues.push_back(stoi(token));
      }catch(exception e){
        throw invalid_argument("value :"+token+" cant be converted to int");
      }
    }
    return composingValues;
}
/************************************************************************/
std::string vectorToString(const std::vector<int>& vec){
    std::ostringstream oss;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            oss << " ";  // Add a space before every element except the first
        }
        oss << vec[i];  // Add the element
    }
    
    return oss.str();
}
/**********************************************************************/
string vector_constant_folding(queue<string> &tokens,unordered_map<string,string>& input_entries){
  while (!tokens.empty())
  {
    if (tokens.front() == "(")
    {
      //std::cout<<"here\n";
      tokens.pop();
      string operationString = tokens.front();
      tokens.pop();
      string potential_step = "";
      string operand1="" ,operand2="";
      if (tokens.front() == "(")
      {
        operand1 = vector_constant_folding(tokens,input_entries);
      }
      else
      {
        operand1 = tokens.front();
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        operand2 = vector_constant_folding(tokens,input_entries);
        potential_step += " ";

      }
      else if (tokens.front() != ")")
      {
        operand2 = tokens.front();
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
        //std::cout<<operationString<<"--"<<operand1<<"--"<<operand2<<" ==> ";
        string type_op1 = operand1.substr(0,1);
        string type_op2 = operand2.substr(0,1);
        //std::cout<<type_op1<<" "<<type_op2<<" \n";
        if(type_op1=="p"&&type_op2=="p"){
            if (input_entries.find(operand1) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand1+" doesnt exist in input_entries");
            }
            if (input_entries.find(operand2) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand2+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand1).substr(0,4);
            vector<int> vec1 = split_string_ints(input_entries.at(operand1).substr(4),' ');
            vector<int> vec2 = split_string_ints(input_entries.at(operand2).substr(4),' ');
             if (vec1.size() != vec2.size()) {
               throw invalid_argument("vectors are of different lengths");
            }
            bool all_vec1_values_eq0 = true ;
            bool all_vec2_values_eq0 = true ;
            for(int i =0;i<vec1.size();i++){
               if(vec1[i]!=0){
                  all_vec1_values_eq0 = false ;
               }
               if(vec2[i]!=0){
                  all_vec2_values_eq0 = false ;
               }
            }
            if(all_vec1_values_eq0&&all_vec2_values_eq0){
                return operand1 ;
            }else if(all_vec1_values_eq0){
                return operand2 ;
            }else if(all_vec2_values_eq0){
               return operand1 ;
            }else{
              // Resize 'values' to match the size of 'vec1' (and 'vec2')
              vector<int> values ;
              values.resize(vec1.size());
              if (operationString=="+"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]+vec2[i];
                }
              }else if(operationString=="-"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]-vec2[i];
                }
              }else if(operationString=="*"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]*vec2[i];
                }
              }
              string new_input = vectorToString(values);
              new_input=header+new_input;
              string label = "p" + std::to_string(id_counter);
              id_counter++;
              input_entries.insert({label,new_input});
              //std::cout<<label<<" \n";
              return label ;
            }
        }else if(type_op1=="p"){
            //std::cout<<"First operand is plaintext \n";
            if (input_entries.find(operand1) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand1+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand1).substr(0,4);
            vector<int> vec1 = split_string_ints(input_entries.at(operand1).substr(4),' ');
            bool all_vec1_values_eq0 = true ;
            bool all_vec1_values_eq1 = true ;
            for(int i =0;i<vec1.size();i++){
               if(vec1[i]!=0){
                  all_vec1_values_eq0 = false ;
               }
               if(vec1[i]!=1){
                  all_vec1_values_eq1 = false ;
               }
            }
            if(all_vec1_values_eq0){
                if(operationString=="+"){
                    return operand2 ;
                }else if (operationString=="*"){
                    return operand1 ;
                }else if (operationString=="-"){
                    string res = "( "+operationString+" "+operand2+" )";
                    return res;
                }else if (operationString=="<<"){
                  return "";
                }
            }else{
              if(all_vec1_values_eq1){
                if (operationString=="*"){
                    return operand2 ;
                }else{
                    string res = "( "+operationString+" "+operand1+" "+operand2+" )" ;
                    return res ;
                }
              }else{
                  string res = "( "+operationString+" "+operand1+" "+operand2+" )" ;
                  return res ;
              }
            }
        }else if(type_op2=="p"){
            //std::cout<<"second operand is plaintext \n";
            if (input_entries.find(operand2) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand2+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand2).substr(0,4);
            vector<int> vec2 = split_string_ints(input_entries.at(operand2).substr(4),' ');
            bool all_vec2_values_eq0 = true ;
            bool all_vec2_values_eq1 = true ;
            for(int i =0;i<vec2.size();i++){
               if(vec2[i]!=0){
                  all_vec2_values_eq0 = false ;
               }
               if(vec2[i]!=1){
                  all_vec2_values_eq1 = false ;
               }
            }
            if(all_vec2_values_eq0){
                if(operationString=="+"){
                    return operand1 ;
                }else if (operationString=="*"){
                    return operand2 ;
                }else if (operationString=="-"){
                    return operand1 ;
                }
            }else{ 
              if(all_vec2_values_eq1){
                if (operationString=="*"){
                    return operand1 ;
                }else{
                    string res = "( "+operationString+" "+operand1+" "+operand2+" )" ;
                    return res ;
                }
              }else{
                  string res = "( "+operationString+" "+operand1+" "+operand2+" )" ;
                  return res ;
              }
            }
        }else{
          string res = "( "+operationString+" "+operand1+" "+operand2+" )" ;
          return res ;
        }
      }
      else 
      {
        return " ( "+operationString+" "+operand1+" )" ;
      }
    }
    else
    {
      return tokens.front();
    }
  }
  throw logic_error("Invalid expressionnn");
}
/************************************************************************/
void update_io_file(const unordered_map<string,string>& input_entries,const vector<string> updated_outputs,int slot_count, int sub_vector_size){
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
    string new_header = std::to_string(slot_count)+" "+std::to_string(input_entries.size())+" "+std::to_string(updated_outputs.size())+"\n";
    updated_input_file << new_header;
    string updated_input ="" ;
    for(const auto&pair : input_entries){
        string vectorString = pair.second.substr(4);
        string addionalInfo = pair.second.substr(0,4);
        updated_input=pair.first+" "+addionalInfo;
        vector<std::string> Valuestokens = split_string(vectorString, ' ');
        if(pair.first.substr(0,1)=="c"){
          for(int i =0; i<Valuestokens.size() ; i++){
              string key = Valuestokens[i];
              if(!is_literal(key)){
                string value =""; 
                if (ciphertexts.find(key) != ciphertexts.end()) {
                    updated_input+=ciphertexts[key];  // Access the value corresponding to the key
                } else {
                    if (plaintexts.find(key) != plaintexts.end()){
                      updated_input+=plaintexts[key];
                    }else{
                      throw invalid_argument("key : "+key+" Not found in ciphertexts and plaintexts map \n");
                    }
                }
              }else{
                updated_input+=key+" ";
              }
          }
        }else if(pair.first.substr(0,1)=="p"){
          for(int i =0; i<Valuestokens.size() ; i++){
              string key = Valuestokens[i];
              if(!is_literal(key)){
                string value =""; 
                if (plaintexts.find(key) != plaintexts.end()){
                  updated_input+=plaintexts[key];
                }else{
                  throw invalid_argument("key : "+key+" Not found in plaintexts map \n");
                }
              }else{
                updated_input+=key+" ";
              }
          }
        }else{
          throw invalid_argument("key :"+pair.first+" is incorrect\n");
        }
        updated_input+="\n";
        //std::cout<<updated_input ;
        updated_input_file << updated_input;
    }
    for(int j=0;j<updated_outputs.size();j++){
        updated_input=updated_outputs[j]+" 1";
        for(int i =0;i<slot_count;i++){
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
    vector<string> simplified_expressions= {};
    string simplified_expression="";
    //std::cout<<"Start Processing of vectorized code :\n" ;
    unordered_map<string,string> inputs_entries ={};
    for (const auto& expr : expressions) {
        if (&expr == &expressions.back()) break;
        auto tokens = process_vectorized_code(expr);
        std::unordered_map<std::string, std::string> dictionary = {};
        //std::cout<<"process expression : "<<expr<<"\n";
        process(tokens,0,dictionary,inputs_entries,inputs,inputs_types, slot_count, sub_vector_size,simplified_expression);
        //std::cout<<"Simplified expression  : "<<simplified_expression<<" \n";
        simplified_expressions.push_back(simplified_expression.substr(1));
        simplified_expression="";
        outputs.push_back(labels_map[id_counter - 1]);
        //std::cout<<"********************************************************\n";
    }
    //string info_tmp ="( + ( * ( + ( * ( * ( + ( * c0 c1 ) ( + ( * c0 c2 ) ( + ( * c0 c3 ) ( + ( * c0 c4 ) ( + ( * c0 c5 ) ( + ( * c0 c6 ) ( * c0 c7 ) ) ) ) ) ) ) c9 ) c11 ) ( + ( * ( + ( * c13 c14 ) ( + ( * c15 c16 ) ( + ( * c17 c18 ) ( + ( * c19 c20 ) ( + ( * c21 c22 ) ( + ( * c23 c24 ) ( + ( * c25 c26 ) ( + ( * c27 c28 ) ( + ( * c29 c30 ) ( + ( * c31 c32 ) ( + ( * c33 c34 ) c35 ) ) ) ) ) ) ) ) ) ) ) p36 ) ( + ( + ( * c38 c39 ) ( + ( * c0 c40 ) ( + ( * c0 c41 ) ( + ( * c0 c42 ) ( + ( * c0 c43 ) ( + ( * c0 c44 ) ( + ( * c0 c45 ) ( + ( * c0 c46 ) ( + ( * c0 c47 ) ( + ( * c0 c48 ) ( + ( * c0 c49 ) ( * c0 c50 ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c0 c52 ) ( + ( * c53 c54 ) ( + ( * c55 c56 ) ( + ( * c0 c57 ) ( + ( * c58 c59 ) ( + ( * c60 c61 ) ( + ( * c62 c63 ) ( + ( * c0 c64 ) ( + ( * c65 c66 ) ( + ( * c67 c68 ) ( + ( * c69 c70 ) ( + ( * c0 c71 ) ( + ( * c72 c73 ) ( + ( * c74 c75 ) ( + ( * c76 c77 ) ( * c0 c78 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ( * c35 p36 ) ) ( * ( + ( * ( * ( + ( * c1 c14 ) ( + ( * c2 c16 ) ( + ( * c4 c22 ) ( + ( * c5 c24 ) ( + ( * c6 c28 ) ( + ( * c90 c30 ) ( + ( * c7 c32 ) c91 ) ) ) ) ) ) ) p92 ) p36 ) ( + ( * ( + ( * c0 c13 ) ( + ( * c0 c15 ) ( + ( * c0 c17 ) ( + ( * c0 c19 ) ( + ( * c0 c21 ) ( + ( * c0 c23 ) ( + ( * c0 c25 ) ( + ( * c0 c27 ) ( + ( * c0 c29 ) ( + ( * c0 c31 ) ( * c0 c33 ) ) ) ) ) ) ) ) ) ) ) c96 ) ( + ( + ( * c0 c38 ) ( + ( * c40 c54 ) ( + ( * c41 c56 ) ( + ( * c0 c58 ) ( + ( * c0 c60 ) ( + ( * c0 c62 ) ( + ( * c0 c65 ) ( + ( * c0 c67 ) ( + ( * c0 c69 ) ( + ( * c0 c72 ) ( + ( * c0 c74 ) ( * c0 c76 ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c52 c39 ) ( + ( * c0 c53 ) ( + ( * c0 c55 ) ( + ( * c57 c99 ) ( + ( * c42 c59 ) ( + ( * c43 c61 ) ( + ( * c44 c63 ) ( + ( * c64 c100 ) ( + ( * c45 c66 ) ( + ( * c46 c68 ) ( + ( * c47 c70 ) ( + ( * c71 c101 ) ( + ( * c48 c73 ) ( + ( * c49 c75 ) ( + ( * c50 c77 ) ( * c78 c102 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c91 p92 ) p36 ) ) )";
    //vector<string> updated_cons_fd_expressions ={} ;
    //updated_cons_fd_expressions = simplified_expressions ;
    //updated_cons_fd_expressions.push_back(info_tmp);
    //std::cout<<"Updated IR : "<<simplified_expression<<" \n";
    /*****************************************************************/
    /*****************************************************************/
    //std::cout<<"applying constant forlding On vectors \n"; 
    vector<string> updated_cons_fd_expressions = simplified_expressions;
    /*for(const auto& expr : simplified_expressions){
        auto tokens1 = split(expr);
        //std::cout<<"Input expression :"<<expr<<" \n";
        string res = vector_constant_folding(tokens1,inputs_entries);
        updated_cons_fd_expressions.push_back(res);
    }*/
    //std::cout<<"==> constant forlding Done\n"; 
    vector<string> labels = {};
    for (const auto& pair : inputs_entries) {
      labels.push_back(pair.first);  // Access the key via pair.first
    }
    unordered_map<string,int> inputs_occurences ={};
    for(auto label : labels){
      inputs_occurences.insert({label,0});
    }
    for(const auto &expr : updated_cons_fd_expressions){
        vector<string> tokens = split_string(expr,' ');
        for(int i =0;i<tokens.size();i++){
          for(auto label : labels){
              if(label==tokens[i]){
                inputs_occurences[label]+=1;
              }
          }
        }        
    }
    for(auto label : labels){
      if(inputs_occurences[label]==0){
        inputs_entries.erase(label);
      }
    }
    /**********************************************************/
    /**********************************************************/
    /*
    Checking if window option is activated ,
    Then we concat exiting outputs in one output 
    */
    if(sub_vector_size<slot_count){
      int nb_outputs = outputs.size();
      string New_IR_expression="";
      for(int i = 0;i<nb_outputs;i++){
        vector<int> mask = vector<int>(slot_count,0);
        for(int j =0; j<sub_vector_size; j++){
          if(i*sub_vector_size+j<slot_count){
            mask[i*sub_vector_size+j]=1;
          }
        }
        // create a new plaintext entrie to store mask
        string label;
        label = "p" + std::to_string(id_counter);
        string new_element = "0 1 " + vectorToString(mask);
        id_counter++;
        inputs_entries[label]=new_element;
        if(i!=(nb_outputs-1)){
           New_IR_expression+="( + ( * "+label+" "+updated_cons_fd_expressions[i]+" ) ";
        }else{
          New_IR_expression+= "( * "+label+" "+updated_cons_fd_expressions[i]+" ) ";
          for(int k=0;k<nb_outputs-1;k++){
            New_IR_expression+=") ";
          }
          New_IR_expression=New_IR_expression.substr(0,New_IR_expression.size()-1);
          //std::cout<<"New IR"<<New_IR_expression<<"||End\n";
          updated_cons_fd_expressions.clear();
          updated_cons_fd_expressions.push_back(New_IR_expression);
          outputs.clear();
          std::string label = "c" + std::to_string(id_counter);
          id_counter++;
          outputs.push_back(label);
        }
      }
    }
    /*****************************************************************/
    /*****************************************************************/
    update_io_file(inputs_entries,outputs,slot_count,sub_vector_size);
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
    //std::cout<<"Creating the new inputs \n";
    for(const auto& new_input_info : inputs_entries){
      //std::cout<<new_input_info<<" \n";
      string label = new_input_info.first ;
      if(label.substr(0,1)=="c"){
          Ciphertext cipher(label);
          func->init_input(cipher,move(label));
      }else{
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
    int index=0;
    //std::cout<<"Number of function inputs : "<<func->data_flow().inputs_info().size()<<"\n";
    //std::cout<<"Start expression traitment ====> \n";
    for(const auto &new_term_str : updated_cons_fd_expressions){
      //std::cout<<new_term_str<<" \n";
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
}
} // namespace fheco
