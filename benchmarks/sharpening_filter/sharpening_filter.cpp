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
  fhecompiler::init("sharpening_filter", 15);

  fhecompiler::Ciphertext img("img", fhecompiler::VarType::input);

  int n = 3;

  fhecompiler::Ciphertext r0 = img * -8;
  fhecompiler::Ciphertext r1 = img << -n - 1;
  fhecompiler::Ciphertext r2 = img << -n;
  fhecompiler::Ciphertext r3 = img << -n + 1;
  fhecompiler::Ciphertext r4 = img << -1;
  fhecompiler::Ciphertext r5 = img << 1;
  fhecompiler::Ciphertext r6 = img << n - 1;
  fhecompiler::Ciphertext r7 = img << n;
  fhecompiler::Ciphertext r8 = img << n + 1;
  fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
  fhecompiler::Ciphertext output2("output2", fhecompiler::VarType::output);

  output = 2 * img - (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8);

  utils::draw_ir(program, "sharpening_filter.hpp1.dot");

  auto count = utils::count_main_node_classes(program);
  for (const auto &e : count)
    cout << e.first << ": " << e.second << endl;

  fheco_passes::CSE cse_pass(program);
  fheco_trs::TRS trs(program);
  trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);
  cse_pass.apply_cse2(true);

  utils::draw_ir(program, "sharpening_filter.hpp2.dot");

  param_selector::ParameterSelector param_selector(program);
  param_selector.select_params();

  utils::draw_ir(program, "sharpening_filter.hpp3.dot");

  cout << endl;

  count = utils::count_main_node_classes(program);
  for (const auto &e : count)
    cout << e.first << ": " << e.second << endl;

  translator::Translator tr(program);
  {
    std::ofstream translation_os("sharpening_filter.hpp");

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate_program(translation_os);

    translation_os.close();
  }

  delete program;
  program = nullptr;

  return 0;
}
