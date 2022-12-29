#include "fhecompiler.hpp"

ir::Program *program;

namespace fhecompiler
{

void init(const std::string &program_name, Scheme program_scheme, Backend backend, double scale)
{
  static ir::Program program_object(program_name);
  program = &program_object;

  // paramters
  params_selector::EncryptionParameters params_object;

  program->set_scheme(program_scheme);
  program->set_targeted_backed(backend);
  program->set_scale(scale);
  // program->set_number_of_slots(number_of_slots);
  if (program_scheme == Scheme::ckks && scale == 0.0)
  {
    throw("scale is missing for CKKS\n");
  }
}

void compile(const std::string &output_filename, params_selector::EncryptionParameters *params)
{
  /*
  params_selector::ParameterSelector parameters_selector(program);
  params_selector::EncryptionParameters params = parameters_selector.select_parameters();
  */
  params_selector::ParameterSelector parameters_selector(program);

  parameters_selector.fix_parameters(*params);

  optimizer::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();

  translator::Translator tr(program, params);
  {
    std::ofstream translation_os(output_filename);

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate(translation_os);

    translation_os.close();
  }
}

} // namespace fhecompiler
