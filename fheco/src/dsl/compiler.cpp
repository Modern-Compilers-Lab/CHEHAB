#include "compiler.hpp"
#include "cse_pass.hpp"
#include "encryption_parameters.hpp"
#include "param_selector.hpp"
#include "relin_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "ruleset.hpp"
#include "translator.hpp"
#include "trs.hpp"
#include <set>

using namespace std;

namespace fhecompiler
{

shared_ptr<ir::Program> Compiler::active_func_;
unordered_map<std::string, std::shared_ptr<ir::Program>> Compiler::funcs_table_;

void Compiler::create_func(const std::string &name, size_t vector_size, int bit_width, bool signedness, Scheme scheme)
{
  auto function_it = funcs_table_.find(name);
  if (function_it != funcs_table_.end())
    throw invalid_argument("funciton with this name already exists");

  active_func_ = make_shared<ir::Program>(name, bit_width, signedness, vector_size, scheme);
  funcs_table_.insert({name, active_func_});
}

void Compiler::delete_func(const std::string &name)
{
  auto function_it = funcs_table_.find(name);
  if (function_it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  funcs_table_.erase(function_it);
  if (active_func_->get_program_tag() == name)
    active_func_ = nullptr;
}

void Compiler::set_active(const std::string &func_name)
{
  auto function_it = funcs_table_.find(func_name);
  if (function_it == funcs_table_.end())
    throw invalid_argument("no function with this name was found");

  active_func_ = function_it->second;
}

void Compiler::compile(
  const shared_ptr<ir::Program> &func, const string &output_file, SecurityLevel sec_level, bool use_mod_switch)
{
  draw_ir(func, output_file + "1.dot");

  fheco_passes::CSE cse_pass(func);
  cse_pass.apply_cse2(true);
  fheco_trs::TRS trs(func);
  trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);
  cse_pass.apply_cse2(true);

  draw_ir(func, output_file + "2.dot");

  param_selector::ParameterSelector param_selector(func, sec_level);
  bool uses_mod_switch = use_mod_switch;
  param_selector::EncryptionParameters params = param_selector.select_params(uses_mod_switch);

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again
  fheco_passes::RotationKeySelctionPass rs_pass(func);
  set<int> rotation_keys_steps = rs_pass.decompose_rotations();

  cse_pass.apply_cse2(true);

  fheco_passes::RelinPass relin_pass(func);
  relin_pass.simple_relinearize();

  draw_ir(func, output_file + "3.dot");

  translator::Translator tr(func, sec_level, params, uses_mod_switch);
  {
    std::ofstream translation_os(output_file);

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate_program(translation_os, rotation_keys_steps);

    translation_os.close();
  }

  draw_ir(func, output_file + "4.dot");
}

} // namespace fhecompiler
