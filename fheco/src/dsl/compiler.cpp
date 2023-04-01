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
#include <variant>

using namespace std;

namespace fhecompiler
{
shared_ptr<ir::Program> Compiler::active_func_;
unordered_map<string, Compiler::FuncEntry> Compiler::funcs_table_;

void Compiler::create_func(const string &name, size_t vector_size, int bit_width, bool signedness, Scheme scheme)
{
  auto function_it = funcs_table_.find(name);
  if (function_it != funcs_table_.end())
    throw invalid_argument("funciton with this name already exists");

  active_func_ = make_shared<ir::Program>(name, bit_width, signedness, vector_size, scheme);
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
}

void Compiler::FuncEntry::compile(ostream &os, int trs_passes, bool use_mod_switch, SecurityLevel sec_level)
{
  fheco_passes::CSE cse_pass(func);
  fheco_trs::TRS trs(func);
  for (int i = 0; i < trs_passes; ++i)
  {
    cse_pass.apply_cse2(true);
    trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);
  }

  fheco_passes::RotationKeySelctionPass rs_pass(func);
  set<int> rotation_keys_steps = rs_pass.decompose_rotations();
  cse_pass.apply_cse2(true);

  fheco_passes::RelinPass relin_pass(func);
  relin_pass.simple_relinearize();

  param_selector::ParameterSelector param_selector(func, sec_level);
  bool uses_mod_switch = use_mod_switch;
  param_selector::EncryptionParameters params = param_selector.select_params(uses_mod_switch);

  translator::Translator tr(func, sec_level, params, uses_mod_switch);
  tr.translate_program(os, rotation_keys_steps);
}

void Compiler::FuncEntry::compile_noopt(ostream &os, SecurityLevel sec_level)
{
  fheco_passes::RotationKeySelctionPass rs_pass(func);
  set<int> rotation_keys_steps = rs_pass.decompose_rotations();

  fheco_passes::RelinPass relin_pass(func);
  relin_pass.simple_relinearize();

  param_selector::ParameterSelector param_selector(func, sec_level);
  bool uses_mod_switch = false;
  param_selector::EncryptionParameters params = param_selector.select_params(uses_mod_switch);

  translator::Translator tr(func, sec_level, params, uses_mod_switch);
  tr.translate_program(os, rotation_keys_steps);
}

void Compiler::FuncEntry::init_input(
  const string &label, const string &tag, long long min_value, long long max_value, ir::VectorValue &destination)
{
  auto node_it = example_inputs_values.find(tag);
  if (node_it != example_inputs_values.end())
    throw logic_error("input node already initialized");

  if (func->get_signedness())
  {
    vector<int64_t> random_value(func->get_vector_size());
    utils::init_random(random_value, min_value, max_value);
    example_inputs_values.insert({tag, random_value});
    destination = random_value;
  }
  else
  {
    vector<uint64_t> random_value(func->get_vector_size());
    utils::init_random(random_value, min_value, max_value);
    example_inputs_values.insert({tag, random_value});
    destination = random_value;
  }
  tags_labels[tag] = label;
}

void Compiler::FuncEntry::init_input(
  const string &label, const string &tag, const ir::VectorValue &example_value, ir::VectorValue &destination)
{
  auto node_it = example_inputs_values.find(tag);
  if (node_it != example_inputs_values.end())
    throw logic_error("input node already initialized");

  const utils::ClearDataEvaluator &evaluator = func->get_clear_data_evaluator();
  destination = visit(
    ir::overloaded{[&evaluator](const auto &value) -> ir::VectorValue {
      return utils::init_const(evaluator, value);
    }},
    example_value);
  tags_labels[tag] = label;
}

void Compiler::FuncEntry::set_output(const string &label, const string &tag, const ir::VectorValue &value)
{
  example_outputs_values[tag] = value;
  tags_labels[tag] = label;
}

void Compiler::FuncEntry::print_inputs_outputs(
  const utils::io_variables_values &inputs, const utils::io_variables_values &outputs, ostream &os) const
{
  if (inputs.size() != example_inputs_values.size())
    throw invalid_argument("invalid number of inputs");

  if (outputs.size() != outputs.size())
    throw invalid_argument("invalid number of outputs");

  ios_base::fmtflags f(os.flags());
  os << boolalpha;

  auto signed_value_var_visitor = [&os](const vector<int64_t> &v) {
    os << true << " ";
    os << v;
  };

  auto unsigned_value_var_visitor = [&os](const vector<uint64_t> &v) {
    os << false << " ";
    os << v;
  };

  os << func->get_vector_size() << " " << example_inputs_values.size() << " " << example_inputs_values.size() << "\n";
  for (const auto &v : inputs)
  {
    if (example_inputs_values.find(v.first) == example_inputs_values.end())
      throw invalid_argument("invalid input tag");

    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("input variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("input node not found in the data flow");

    os << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(ir::overloaded{signed_value_var_visitor, unsigned_value_var_visitor}, v.second);
    os << "\n";
  }
  for (const auto &v : outputs)
  {
    if (example_inputs_values.find(v.first) == example_inputs_values.end())
      throw invalid_argument("invalid output tag");

    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("output variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("output node not found in the data flow");

    os << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(ir::overloaded{signed_value_var_visitor, unsigned_value_var_visitor}, v.second);
    os << "\n";
  }
  os.flags(f);
}

void Compiler::FuncEntry::print_inputs_outputs(ostream &os) const
{
  ios_base::fmtflags f(os.flags());
  os << boolalpha;

  auto signed_value_var_visitor = [&os](const vector<int64_t> &v) {
    os << true << " ";
    os << v;
  };

  auto unsigned_value_var_visitor = [&os](const vector<uint64_t> &v) {
    os << false << " ";
    os << v;
  };

  os << func->get_vector_size() << " " << example_inputs_values.size() << " " << example_inputs_values.size() << "\n";
  for (const auto &v : example_inputs_values)
  {
    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("input variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("input node not found in the data flow");

    os << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(ir::overloaded{signed_value_var_visitor, unsigned_value_var_visitor}, v.second);
    os << "\n";
  }
  for (const auto &v : example_inputs_values)
  {
    auto v_label_it = tags_labels.find(v.first);
    if (v_label_it == tags_labels.end())
      throw logic_error("output variable without label in tags_labels");

    const auto &node = func->find_node_in_dataflow(v_label_it->second);
    if (!node)
      throw logic_error("output node not found in the data flow");

    os << v.first << " " << (node->get_term_type() == ir::TermType::ciphertext) << " ";
    visit(ir::overloaded{signed_value_var_visitor, unsigned_value_var_visitor}, v.second);
    os << "\n";
  }
  os.flags(f);
}
} // namespace fhecompiler
