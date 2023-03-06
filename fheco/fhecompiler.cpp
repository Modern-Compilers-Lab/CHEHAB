#include "fhecompiler.hpp"
#include "cse_pass.hpp"
#include "draw_ir.hpp"
#include "normalize_pass.hpp"
#include "relin_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "trs.hpp"
#include "trs_util_functions.hpp"

#define NB_TRS_CSE_PASS 6
#define ENABLE_OPTIMIZATION true

using namespace fhecompiler;

ir::Program *program;

namespace fhecompiler
{

void init(const std::string &program_name, Scheme program_scheme, Backend backend, size_t vector_size, double scale)
{

  if (program != nullptr)
    delete program;

  program = new ir::Program(program_name);

  program->set_scheme(program_scheme);
  program->set_targeted_backed(backend);
  program->set_scale(scale);
  program->set_vector_size(vector_size);
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

  // utils::draw_ir(program, output_filename + "0.dot");

  fheco_passes::CSE cse_pass(program);

  if (ENABLE_OPTIMIZATION == true)
    cse_pass.apply_cse2();

  // fheco_passes::Normalizer normalizer(program);
  // normalizer.normalize();

  // utils::draw_ir(program, output_filename + "1.dot");

  fheco_trs::TRS trs(program);

  if (ENABLE_OPTIMIZATION)
  {
    for (size_t i = NB_TRS_CSE_PASS; i > 0; i--)
    {
      // utils::draw_ir(program, output_filename + std::to_string(i) + ".dot");
      trs.apply_rewrite_rules_on_program_from_static_ruleset();
      cse_pass.apply_cse2();
    }
  }

  // utils::draw_ir(program, output_filename + std::to_string(NB_TRS_CSE_PASS) + ".dot");

  utils::draw_ir(program, output_filename + "2.dot");

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again

  fheco_passes::RotationKeySelctionPass rs_pass(program, params);
  rs_pass.collect_program_rotations_steps();

  if (ENABLE_OPTIMIZATION)
    cse_pass.apply_cse2();

  ir::print_ops_counters(program);

  fheco_passes::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();

  translator::Translator tr(program, params);
  {
    std::ofstream translation_os(output_filename);

    if (!translation_os)
      throw("couldn't open file for translation.\n");

    tr.translate_program(translation_os);

    translation_os.close();
  }
  delete program;
  program = nullptr;
}

} // namespace fhecompiler
