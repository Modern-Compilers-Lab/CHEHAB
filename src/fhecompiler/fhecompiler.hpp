#pragma once

#include "ciphertext.hpp"
#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "noise_simulator.hpp"
#include "ops_overloads.hpp"
#include "optimizer.hpp"
#include "params_selector.hpp"
#include "plaintext.hpp"
#include "program.hpp"
#include "scalar.hpp"
#include "translator.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

ir::Program *program;

namespace fhecompiler
{

static params_selector::EncryptionParameters params;

void init(
  const std::string &program_name, size_t plaintext_modulus, Scheme program_scheme, Backend backend = Backend::SEAL)
{
  static ir::Program program_object(program_name);
  program = &program_object;
  program->set_scheme(program_scheme);
  program->set_targeted_backed(backend);
}

void compile(const std::string &output_filename)
{
  /*
  params_selector::ParameterSelector parameters_selector(program);
  params_selector::EncryptionParameters params = parameters_selector.select_parameters();
  */

  params_selector::ParameterSelector parameters_selector(program);

  parameters_selector.fix_parameters(params);
  /*
  noise_simulator::BGVSimulator bgv_simulator(8192, 786433, 700);

  bgv_simulator.simulate_noise_growth(program);
  */

  // optimizer::RelinPass relin_pass(program);
  //  relin_pass.simple_relinearize();

  translator::Translator tr(program, &params);
  {
    std::ofstream translation_os(output_filename);

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate(translation_os);

    translation_os.close();
  }
}

} // namespace fhecompiler
