#include "compiler.hpp"
#include "cse_pass.hpp"
#include "encryption_parameters.hpp"
#include "param_selector.hpp"
#include "relin_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "ruleset.hpp"
#include "translator.hpp"
#include "trs.hpp"
#include <cstdint>
#include <fstream>
#include <set>
#include <type_traits>
#include <variant>

using namespace std;

namespace fhecompiler
{

shared_ptr<ir::Program> Compiler::active_func_;
unordered_map<string, Compiler::FuncEntry> Compiler::funcs_table_;
utils::ClearDataEvaluator Compiler::clear_data_evaluator;

void Compiler::create_func(const string &name, size_t vector_size, int bit_width, bool signedness, Scheme scheme)
{
  auto function_it = funcs_table_.find(name);
  if (function_it != funcs_table_.end())
    throw invalid_argument("funciton with this name already exists");

  active_func_ = make_shared<ir::Program>(name, bit_width, signedness, vector_size, scheme);
  clear_data_evaluator = utils::ClearDataEvaluator(vector_size, bit_width, signedness);
  funcs_table_.insert({name, {active_func_}});
}

void Compiler::delete_func(const string &name)
{
  auto function_it = funcs_table_.find(name);
  if (function_it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  funcs_table_.erase(function_it);
  if (active_func_->get_program_tag() == name)
    active_func_ = nullptr;
}

void Compiler::set_active(const string &func_name)
{
  auto function_it = funcs_table_.find(func_name);
  if (function_it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  active_func_ = function_it->second.func;
  clear_data_evaluator = utils::ClearDataEvaluator(
    active_func_->get_vector_size(), active_func_->get_bit_width(), active_func_->get_signedness());
}

void Compiler::compile(
  const shared_ptr<ir::Program> &func, const string &output_file, SecurityLevel sec_level, bool use_mod_switch)
{
  draw_ir(func, func->get_program_tag() + "_naive_ir.dot");

  fheco_passes::CSE cse_pass(func);
  cse_pass.apply_cse2(true);
  fheco_trs::TRS trs(func);
  trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);
  cse_pass.apply_cse2(true);

  param_selector::ParameterSelector param_selector(func, sec_level);
  bool uses_mod_switch = use_mod_switch;
  param_selector::EncryptionParameters params = param_selector.select_params(uses_mod_switch);

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again
  fheco_passes::RotationKeySelctionPass rs_pass(func);
  set<int> rotation_keys_steps = rs_pass.decompose_rotations();

  cse_pass.apply_cse2(true);

  fheco_passes::RelinPass relin_pass(func);
  relin_pass.simple_relinearize();

  translator::Translator tr(func, sec_level, params, uses_mod_switch);
  ofstream translation_os(output_file);

  if (!translation_os)
    throw("couldn't open file for translation.\n");

  tr.translate_program(translation_os, rotation_keys_steps);

  translation_os.close();

  draw_ir(func, func->get_program_tag() + "_opt_ir.dot");
}

const utils::variables_values_map::mapped_type &Compiler::FuncEntry::get_node_value(const std::string &label) const
{
  auto node_value_it = nodes_values.find(label);
  if (node_value_it == nodes_values.end())
    throw invalid_argument("node value not available");

  return node_value_it->second;
}

void Compiler::FuncEntry::init_input(const string &label, const string &tag)
{
  auto node_it = nodes_values.find(label);
  if (node_it != nodes_values.end())
    throw logic_error("input node already initialized");

  if (func->get_signedness())
  {
    vector<int64_t> random_value(func->get_vector_size());
    utils::init_random(random_value, -100, 100);
    nodes_values.insert({label, random_value});
    inputs_values.insert({tag, random_value});
  }
  else
  {
    vector<uint64_t> random_value(func->get_vector_size());
    utils::init_random(random_value, 0, 100);
    utils::variables_values_map::value_type node_entry = {label, random_value};
    nodes_values.insert({label, random_value});
    inputs_values.insert({tag, random_value});
  }
  tags_labels[tag] = label;
}

void Compiler::FuncEntry::add_const_node_value(
  const string &label, const utils::variables_values_map::mapped_type &value)
{
  auto node_it = nodes_values.find(label);
  if (node_it != nodes_values.end())
    throw logic_error("node value already added");

  utils::variables_values_map::value_type node_entry = {label, value};
  nodes_values.insert(node_entry);
}

void Compiler::FuncEntry::operate_unary(
  ir::OpCode op, const string &arg, const std::string &destination, bool is_output)
{
  // auto destination_value_it = nodes_values.find(destination);
  // if (destination_value_it != nodes_values.end())
  //   throw logic_error("destination node already has a value");

  auto arg_value_var = get_node_value(arg);
  if (auto arg_value = get_if<vector<int64_t>>(&arg_value_var))
  {
    switch (op)
    {
    case ir::OpCode::encrypt:
    case ir::OpCode::assign:
      nodes_values[destination] = *arg_value;
      break;
    case ir::OpCode::negate:
    {
      vector<int64_t> destination_value;
      clear_data_evaluator.negate(*arg_value, destination_value);
      nodes_values[destination] = destination_value;
      break;
    }
    case ir::OpCode::square:
    {
      vector<int64_t> destination_value;
      clear_data_evaluator.multiply(*arg_value, *arg_value, destination_value);
      nodes_values[destination] = destination_value;
      break;
    }
    default:
      throw logic_error("unhandled unary operation");
      break;
    }
  }
  else if (auto arg_value = get_if<vector<uint64_t>>(&arg_value_var))
  {
    switch (op)
    {
    case ir::OpCode::encrypt:
    case ir::OpCode::assign:
      nodes_values[destination] = *arg_value;
      break;
    case ir::OpCode::negate:
    {
      vector<uint64_t> destination_value;
      clear_data_evaluator.negate(*arg_value, destination_value);
      nodes_values[destination] = destination_value;
      break;
    }
    case ir::OpCode::square:
    {
      vector<uint64_t> destination_value;
      clear_data_evaluator.multiply(*arg_value, *arg_value, destination_value);
      nodes_values[destination] = destination_value;
      break;
    }
    default:
      throw logic_error("unhandled unary operation");
      break;
    }
  }
  else
    throw logic_error("could not get child value");

  if (is_output)
  {
    try
    {
      string tag = func->get_tag_value_in_constants_table_entry(destination);
      outputs_values[tag] = nodes_values[destination];
      tags_labels[tag] = destination;
    }
    catch (const std::string &e)
    {
      throw logic_error("output node without entry in const table (no tag)");
    }
  }
}

void Compiler::FuncEntry::operate_binary(
  ir::OpCode op, const string &arg1, const string &arg2, const std::string &destination, bool is_output)
{
  // auto destination_value_it = nodes_values.find(destination);
  // if (destination_value_it != nodes_values.end())
  //   throw logic_error("destination node already has a value");

  auto arg1_value_var = get_node_value(arg1);
  auto arg2_value_var = get_node_value(arg2);

  switch (op)
  {
  case ir::OpCode::add:
  {
    if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<int64_t> destination_value;
        clear_data_evaluator.add(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.add(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.add(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.add(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else
      throw logic_error("could not get child value");
    break;
  }
  case ir::OpCode::sub:
  {
    if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<int64_t> destination_value;
        clear_data_evaluator.sub(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.sub(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.sub(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.sub(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else
      throw logic_error("could not get child value");
    break;
  }
  case ir::OpCode::mul:
  {
    if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<int64_t> destination_value;
        clear_data_evaluator.multiply(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.multiply(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_var))
    {
      if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.multiply(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_var))
      {
        vector<uint64_t> destination_value;
        clear_data_evaluator.multiply(*arg1_value, *arg2_value, destination_value);
        nodes_values[destination] = destination_value;
      }
      else
        throw logic_error("could not get child value");
    }
    else
      throw logic_error("could not get child value");
    break;
  }
  default:
    throw logic_error("unhandled binary operation");
    break;
  }
  if (is_output)
  {
    try
    {
      string tag = func->get_tag_value_in_constants_table_entry(destination);
      outputs_values[tag] = nodes_values[destination];
      tags_labels[tag] = destination;
    }
    catch (const std::string &e)
    {
      throw logic_error("output node without entry in const table (no tag)");
    }
  }
}

void Compiler::FuncEntry::operate_rotate(const string &arg, int step, const std::string &destination, bool is_output)
{
  // auto destination_value_it = nodes_values.find(destination);
  // if (destination_value_it != nodes_values.end())
  //   throw logic_error("destination node already has a value");

  auto arg_value_var = get_node_value(arg);
  if (auto arg_value = get_if<vector<int64_t>>(&arg_value_var))
  {
    vector<int64_t> destination_value;
    clear_data_evaluator.rotate(*arg_value, step, destination_value);
    nodes_values[destination] = destination_value;
  }
  else if (auto arg_value = get_if<vector<uint64_t>>(&arg_value_var))
  {
    vector<uint64_t> destination_value;
    clear_data_evaluator.rotate(*arg_value, step, destination_value);
    nodes_values[destination] = destination_value;
  }
  else
    throw logic_error("could not get child value");

  if (is_output)
  {
    try
    {
      string tag = func->get_tag_value_in_constants_table_entry(destination);
      outputs_values[tag] = nodes_values[destination];
      tags_labels[tag] = destination;
    }
    catch (const std::string &e)
    {
      throw logic_error("output node without entry in const table (no tag)");
    }
  }
}

void Compiler::FuncEntry::serialize_inputs_outputs(
  const utils::variables_values_map &inputs, const utils::variables_values_map &outputs, const string &file_name) const
{
  if (inputs.size() != inputs_values.size())
    throw invalid_argument("invalid number of inputs");

  if (outputs.size() != outputs.size())
    throw invalid_argument("invalid number of outputs");

  ofstream file(file_name);
  auto value_var_visitor = [&file](const auto &v) {
    using T = decay_t<decltype(v)>;
    if constexpr (is_same_v<T, vector<int64_t>>)
      file << 1 << " ";
    else if constexpr (std::is_same_v<T, vector<uint64_t>>)
      file << 0 << " ";
    else
      static_assert(utils::always_false_v<T>, "non-exhaustive visitor!");

    utils::serialize_vector(v, file);
  };

  file << func->get_vector_size() << " " << inputs_values.size() << " " << outputs_values.size() << endl;
  for (const auto &v : inputs)
  {
    if (inputs_values.find(v.first) == inputs_values.end())
      throw invalid_argument("invalid input tag");

    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("input variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("input node not found in the data flow");

    file << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(value_var_visitor, v.second);
    file << endl;
  }
  for (const auto &v : outputs)
  {
    if (outputs_values.find(v.first) == outputs_values.end())
      throw invalid_argument("invalid output tag");

    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("output variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("output node not found in the data flow");

    file << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(value_var_visitor, v.second);
    file << endl;
  }
}

void Compiler::FuncEntry::serialize_inputs_outputs(const string &file_name) const
{
  ofstream file(file_name);
  auto value_var_visitor = [&file](const auto &v) {
    using T = decay_t<decltype(v)>;
    if constexpr (is_same_v<T, vector<int64_t>>)
      file << 1 << " ";
    else if constexpr (std::is_same_v<T, vector<uint64_t>>)
      file << 0 << " ";
    else
      static_assert(utils::always_false_v<T>, "non-exhaustive visitor!");

    utils::serialize_vector(v, file);
  };

  file << func->get_vector_size() << " " << inputs_values.size() << " " << outputs_values.size() << endl;
  for (const auto &v : inputs_values)
  {
    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("input variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("input node not found in the data flow");

    file << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(value_var_visitor, v.second);
    file << endl;
  }
  for (const auto &v : outputs_values)
  {
    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("output variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("output node not found in the data flow");

    file << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(value_var_visitor, v.second);
    file << endl;
  }
}

} // namespace fhecompiler
