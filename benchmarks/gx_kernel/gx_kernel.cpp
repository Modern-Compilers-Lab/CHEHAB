#include "fhecompiler/cse_pass.hpp"
#include "fhecompiler/draw_ir.hpp"
#include "fhecompiler/fhecompiler.hpp"
#include "fhecompiler/normalize_pass.hpp"
#include "fhecompiler/param_selector.hpp"
#include "fhecompiler/quantify_ir.hpp"
#include "fhecompiler/ruleset.hpp"
#include "fhecompiler/trs.hpp"
#include "fhecompiler/trs_util_functions.hpp"
#include <iostream>

using namespace std;

extern ir::Program *program;

int main()
{
  fhecompiler::init("gx_kernel", 15);

  fhecompiler::Ciphertext c0("c0", fhecompiler::VarType::input);

  fhecompiler::Ciphertext c1 = c0 << 1;
  fhecompiler::Ciphertext c2 = c0 << 5;
  fhecompiler::Ciphertext c3 = c0 << 6;
  fhecompiler::Ciphertext c4 = c0 << -1;
  fhecompiler::Ciphertext c5 = c0 << -4;
  fhecompiler::Ciphertext c6 = c0 << -6;
  fhecompiler::Ciphertext c7 = c1 + c2;
  fhecompiler::Ciphertext c8 = c3 + c4;
  fhecompiler::Ciphertext c9 = c5 + c6;
  fhecompiler::Ciphertext c10 = c7 + c8;
  fhecompiler::Ciphertext c11 = c9 + c9;
  fhecompiler::Ciphertext c12 = c10 + c11;
  fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
  output = c12;

  utils::draw_ir(program, "gx_kernel.hpp1.dot");

  auto count = utils::count_main_node_classes(program);
  for (const auto &e : count)
    cout << e.first << ":" << e.second << endl;

  fheco_passes::CSE cse_pass(program);
  fheco_trs::TRS trs(program);
  trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);

  cse_pass.apply_cse2(true);
  utils::draw_ir(program, "gx_kernel.hpp2.dot");

  cout << endl;

  param_selector::ParameterSelector param_selector(program);
  param_selector.select_params();

  utils::draw_ir(program, "gx_kernel.hpp3.dot");

  count = utils::count_main_node_classes(program);
  for (const auto &e : count)
    cout << e.first << ": " << e.second << endl;

  translator::Translator tr(program);
  {
    std::ofstream translation_os("gx_kernel.hpp");

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate_program(translation_os);

    translation_os.close();
  }

  delete program;
  program = nullptr;
  return 0;
}
