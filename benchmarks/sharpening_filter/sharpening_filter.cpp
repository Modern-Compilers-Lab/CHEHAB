#include "fhecompiler/cse_pass.hpp"
#include "fhecompiler/draw_ir.hpp"
#include "fhecompiler/fhecompiler.hpp"
#include "fhecompiler/normalize_pass.hpp"
#include "fhecompiler/quantify_ir.hpp"
#include "fhecompiler/ruleset.hpp"
#include "fhecompiler/trs.hpp"
#include "fhecompiler/trs_util_functions.hpp"
#include <iostream>

using namespace std;

extern ir::Program *program;

int main()
{
  try
  {
    fhecompiler::init("sharpening_filter", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);
    program->set_number_of_slots(8192);

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

    cout << endl;

    count = utils::count_main_node_classes(program);
    for (const auto &e : count)
      cout << e.first << ": " << e.second << endl;

    delete program;
    program = nullptr;
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
