#include "fhecompiler.hpp"
#include "relin_pass.hpp"
#include "rotation_rewrite_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "test/dummy_ruleset.hpp"
#include "trs.hpp"

using namespace fhecompiler;

ir::Program *program;

namespace fhecompiler
{

void init(const std::string &program_name, Scheme program_scheme, Backend backend, double scale)
{
  if (program != nullptr)
    delete program;

  program = new ir::Program(program_name);

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

  // compact assignement pass

  {
    const std::vector<Ptr> &nodes_ptr = program->get_dataflow_sorted_nodes(true);

    for (auto &node_ptr : nodes_ptr)
    {
      program->compact_assignement(node_ptr);
    }
  }

  fheco_trs::TRS trs(program);
  trs.apply_trs_rewrite_rules_on_program(fheco_trs::dummy_ruleset);

  fheco_passes::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();

  // fheco_passes::RotationRewritePass rw_pass(program);
  // rw_pass.apply_rewrite();

  /*
    std::vector<int> program_rotations_steps = fheco_passes::get_unique_rotation_steps(program);
    for (auto &step : program_rotations_steps)
      std::cout << step << " ";
    std::cout << "\n";
  */

  /*
  // convert to inplace pass
  {
    const std::vector<Ptr> &nodes_ptr = program->get_dataflow_sorted_nodes(true);

    for (auto &node_ptr : nodes_ptr)
    {
      program->convert_to_inplace(node_ptr);
    }

    // this needs to be the last pass and we must not do any new sorting of the graph
  }
  */

  translator::Translator tr(program, params);
  {
    std::ofstream translation_os(output_filename);

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate(translation_os);

    translation_os.close();
  }
  delete program;
  program = nullptr;
}

} // namespace fhecompiler
