#include "fhecompiler.hpp"
#include "cse_pass.hpp"
#include "draw_ir.hpp"
#include "normalize_pass.hpp"
#include "param_selector.hpp"
#include "relin_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "ruleset.hpp"
#include "trs.hpp"
#include "trs_util_functions.hpp"

using namespace fhecompiler;

ir::Program *program;

namespace fhecompiler
{

void init(
  const std::string &name, int bit_width, bool signedness, std::size_t vec_size, SecurityLevel sec_level, Scheme scheme,
  double scale)
{

  if (program != nullptr)
    delete program;

  program = new ir::Program(name);

  program->set_bit_width(bit_width);
  program->set_signedness(signedness);
  program->set_vector_size(vec_size);
  program->set_sec_level(sec_level);
  program->set_scheme(scheme);
  program->set_scale(scale);

  if (scheme == Scheme::ckks && scale == 0.0)
  {
    throw("scale is missing for CKKS\n");
  }
}

void compile(const std::string &output_filename)
{

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

  // cse_pass.apply_cse2(false);

  fheco_passes::Normalizer normalizer(program);
  normalizer.normalize();

  utils::draw_ir(program, output_filename + "1.dot");

  fheco_trs::TRS trs(program);

  trs.apply_rewrite_rules_on_program(fheco_trs::Ruleset::rules);

  cse_pass.apply_cse2(true);

  param_selector::ParameterSelector param_selector(program);
  param_selector.select_params();

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again
  fheco_passes::RotationKeySelctionPass rs_pass(program);
  rs_pass.decompose_rotations();

  cse_pass.apply_cse2(true);

  fheco_passes::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();

  // utils::draw_ir(program, output_filename + "2.dot");

  translator::Translator tr(program);
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
