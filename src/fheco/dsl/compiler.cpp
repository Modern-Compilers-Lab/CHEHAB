
#include "fheco/code_gen/gen_func.hpp"
#include "fheco/dsl/compiler.hpp"
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
#include <sstream>
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

void Compiler::compile(
  shared_ptr<ir::Func> func, ostream &header_os, string_view header_name, ostream &source_os, bool axiomatic,
  int window)
{

#ifdef FHECO_LOGGING
  clog << "\ncse_commut\n";
#endif
  if (window)
    equality_saturation_optimizer(func, axiomatic, window);
  else
    equality_saturation_optimizer(func, axiomatic);
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

queue<string> Compiler::split(const string &s)
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
ir::Term *Compiler::build_expression(
  const std::shared_ptr<ir::Func> &func, map<string, ir::Term *> map, queue<string> &tokens)
{
  ;
  while (!tokens.empty())
  {

    if (tokens.front() == "(")
    {

      bool op_is_rotation = false;
      tokens.pop();

      string operationString = tokens.front();
      ir::OpCode operation = ir::OpCode::nop;
      if (operationString != "<<")
        operation = operationFromString(operationString);
      else
        op_is_rotation = true;
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
        if (!op_is_rotation)
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
        else
        {
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
ir::Term *Compiler::build_expression(
  const std::shared_ptr<ir::Func> &func, std::map<string, ir::Term *> map, queue<string> &tokens, int window, int depth,
  bool axiomatic)
{
  while (depth < window)
  {

    if (tokens.front() == "(")
    {
      bool op_is_rotation = false;
      tokens.pop();
      string operationString = tokens.front();

      ir::OpCode operation = ir::OpCode::nop;

      if (operationString != "<<")
        operation = operationFromString(operationString);
      else
        op_is_rotation = true;
      tokens.pop();

      string potential_step = "";

      ir::Term *operand1, *operand2 = nullptr;
      if (tokens.front() == "(")
      {

        operand1 = build_expression(func, map, tokens, window, depth + 1, axiomatic);
      }
      else
      {

        operand1 = map.at(tokens.front());

        tokens.pop();
      }
      if (tokens.front() == "(")
      {

        operand2 = build_expression(func, map, tokens, window, depth + 1, axiomatic);
      }
      else if (tokens.front() != ")")
      {
        if (!op_is_rotation)
        {

          operand2 = map.at(tokens.front());
        }
        else
          potential_step = tokens.front();

        tokens.pop();
      }

      // Check for the closing parenthesis
      if (tokens.front() == ")")
      {
        tokens.pop();
      }

      if (potential_step.size() > 0 || operand2)
      {

        if (op_is_rotation)
        {

          operation = ir::OpCode::rotate(stoi(potential_step));

          vector<ir::Term *> operands = {operand1};

          return func->insert_op_term(move(operation), move(operands));
        }
        else
        {

          vector<ir::Term *> operands = {operand1, operand2};

          auto term = func->insert_op_term(move(operation), move(operands));
          return term;
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

  int expression_depth = 1;
  string subterm = "(";
  tokens.pop();
  while (expression_depth > 0)
  {
    subterm = subterm + " " + tokens.front();
    if (tokens.front() == "(")
      expression_depth += 1;
    else if (tokens.front() == ")")
      expression_depth -= 1;
    tokens.pop();
  }
  ofstream expression_file("../expression.txt");
  expression_file << subterm;
  expression_file.close();
  call_equality_saturation(func->slot_count(), axiomatic);
  std::ifstream simplified_file("../simplified_expression.txt");
  std::stringstream buffer;
  buffer << simplified_file.rdbuf();
  std::string simplified_expression = buffer.str();
  simplified_file.close();
  auto tokens_sub_term = split(simplified_expression);
  return build_expression(func, map, tokens_sub_term);
}
void Compiler::equality_saturation_optimizer(const std::shared_ptr<ir::Func> &func, bool axiomatic)
{

  util ::ExprPrinter pr(func);
  pr.make_terms_str_expr(util::ExprPrinter::Mode::prefix);
  map<string, ir::Term *> myMap;
  for (auto input_info : func->data_flow().inputs_info())
  {
    ir::Term *temp = const_cast<ir::Term *>(input_info.first);

    myMap[input_info.second.label_] = temp;
  }
  for (auto const_info : func->data_flow().constants_info())
  {

    ir::Term *temp = const_cast<ir::Term *>(const_info.first);
    auto it = pr.terms_str_exprs().find(const_info.first->id());
    if (it != pr.terms_str_exprs().end())
      myMap[pr.terms_str_exprs().at(const_info.first->id())] = temp;
  }

  vector<const ir::Term *> output_terms;
  for (auto output_info : func->data_flow().outputs_info())
  {

    output_terms.push_back(output_info.first);
  }

  for (auto output_term : output_terms)
  {
    ofstream expression_file("../expression.txt");
    expression_file << pr.terms_str_exprs().at(output_term->id());
    expression_file.close();
    call_equality_saturation(func->slot_count(), axiomatic);
    std::ifstream simplified_file("../simplified_expression.txt");
    std::stringstream buffer;
    buffer << simplified_file.rdbuf();
    std::string simplified_expression = buffer.str();
    simplified_file.close();
    auto tokens = split(simplified_expression);
    auto new_term = build_expression(func, myMap, tokens);
    auto old_term = const_cast<ir::Term *>(output_term);

    func->replace_term_with(old_term, new_term);
  }
}

void Compiler::equality_saturation_optimizer(const std::shared_ptr<ir::Func> &func, bool axiomatic, int window)
{
  util ::ExprPrinter pr(func);
  pr.make_terms_str_expr(util::ExprPrinter::Mode::prefix);
  map<string, ir::Term *> myMap;
  for (auto input_info : func->data_flow().inputs_info())
  {
    ir::Term *temp = const_cast<ir::Term *>(input_info.first);

    myMap[input_info.second.label_] = temp;
  }
  for (auto const_info : func->data_flow().constants_info())
  {

    ir::Term *temp = const_cast<ir::Term *>(const_info.first);
    auto it = pr.terms_str_exprs().find(const_info.first->id());
    if (it != pr.terms_str_exprs().end())
      myMap[pr.terms_str_exprs().at(const_info.first->id())] = temp;
  }

  vector<const ir::Term *> output_terms;
  for (auto output_info : func->data_flow().outputs_info())
  {

    output_terms.push_back(output_info.first);
  }

  for (auto output_term : output_terms)
  {

    string expression = pr.terms_str_exprs().at(output_term->id());
    auto tokens = split(expression);
    call_equality_saturation(func->slot_count(), axiomatic);
    auto new_term = build_expression(func, myMap, tokens, window, 0, axiomatic);
    auto old_term = const_cast<ir::Term *>(output_term);

    func->replace_term_with(old_term, new_term);
  }
}
void Compiler::call_equality_saturation(int slot_count, bool axiomatic)
{
  int axiomatic_int = axiomatic ? 1 : 0;
  string command = "cargo run --release --manifest-path ../../../egraphs/Cargo.toml -- ../expression.txt " +
                   to_string(slot_count) + " " + to_string(axiomatic_int) + " > ../simplified_expression.txt";

  // Use the system function to run the executable
  int result = system(command.c_str());

  // Check the result of the system call
  if (result != 0)
  {
    // The executable ran successfully
    cout << "Failed to call equality saturation !" << endl;
  }
}

} // namespace fheco
