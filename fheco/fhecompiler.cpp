#include "fhecompiler.hpp"
#include "additional_passes.hpp"
#include "cse_pass.hpp"
#include "draw_ir.hpp"
#include "normalize_pass.hpp"
#include "param_selector.hpp"
#include "relin_pass.hpp"
#include "rotationkeys_select_pass.hpp"
#include "trs.hpp"
#include "trs_util_functions.hpp"

#define NB_TRS_CSE_PASS 30
#define ENABLE_OPTIMIZATION true

using namespace fhecompiler;

ir::Program *program;

namespace fhecompiler
{

void init(
  const std::string &name, int bit_width, bool signedness, std::size_t vec_size, SecurityLevel sec_level, Scheme scheme,
  double scale, fhecompiler::Backend targeted_backend)
{
  std::cout << "init 26\n";
  if (program != nullptr)
    delete program;

  program = new ir::Program(name);

  program->set_bit_width(bit_width);
  program->set_signedness(signedness);
  program->set_vector_size(vec_size);
  program->set_sec_level(sec_level);
  program->set_scheme(scheme);
  program->set_scale(scale);
  program->set_targeted_backed(targeted_backend);

  std::cout << "init 40\n";

  if (scheme == Scheme::ckks && scale == 0.0)
  {
    throw("scale is missing for CKKS\n");
  }

  std::cout << "init 47\n";
}

void compile(const std::string &output_filename)
{
  std::cout << "compiling ...\n";
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

  // if (ENABLE_OPTIMIZATION == true)
  cse_pass.apply_cse2();

  std::cout << "cse passed...\n";

  utils::draw_ir(program, output_filename + "1.dot");

  // fheco_passes::Normalizer normalizer(program);
  // normalizer.normalize();

  fheco_trs::TRS trs(program);
  /*
  {
    using T = fheco_trs::MatchingTerm;
    T x(fheco_trs::TermType::ciphertextType);
    T p(fheco_trs::TermType::rawDataType);
    T q(fheco_trs::TermType::rawDataType);

    std::vector<fheco_trs::RewriteRule> rotations_ruleset = {
      {(x << p), x, p == 0}, {(x << p) << q, x << T::fold(p + q)}};

    trs.apply_rewrite_rules_on_program(rotations_ruleset);
    cse_pass.apply_cse2();
  }
  */

  // fheco_passes::optimize_SOR_pass2(program, ir::OpCode::add);
  // /cse_pass.apply_cse2();

  if (ENABLE_OPTIMIZATION)
  {
    for (size_t i = NB_TRS_CSE_PASS; i > 0; i--)
    {
      // utils::draw_ir(program, output_filename + std::to_string(i) + ".dot");
      trs.apply_rewrite_rules_on_program_from_static_ruleset();
      cse_pass.apply_cse2();
      std::cout << "pass# " << i << "\n";
    }
  }
  else
    std::cout << "optimization is disabled\n";

  // utils::draw_ir(program, output_filename + std::to_string(NB_TRS_CSE_PASS) + ".dot");

  // utils::draw_ir(program, output_filename + "2.dot");

  // fheco_passes::RotationKeySelctionPass rs_pass(program, params);
  // rs_pass.collect_program_rotations_steps();

  if (ENABLE_OPTIMIZATION)
    cse_pass.apply_cse2();

  ir::print_ops_counters(program);

  param_selector::ParameterSelector param_selector(program);
  param_selector.select_params(false);

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again
  fheco_passes::RotationKeySelctionPass rs_pass(program);
  rs_pass.decompose_rotations();

  fheco_passes::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();

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
