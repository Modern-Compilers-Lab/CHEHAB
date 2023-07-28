#include "fhecompiler.hpp"
#include "additional_passes.hpp"
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

  std::cout << "La représentation intermédiare a été construite \n \n";

  fheco_passes::CSE cse_pass(program);

  // if (ENABLE_OPTIMIZATION == true)

  std::cout << "Calcul de l'état initial du circuit ...\n \n";

  cse_pass.apply_cse2();

  ir::print_program_depth(program);

  ir::print_ops_counters(program);

  std::cout << std::string(50, '-') << "\n \n";

  std::cout << "compilation...\n \n";

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

  // utils::draw_ir(program, output_filename + "1.dot");

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
    std::cout << "TRS et CSE ...\n";
    for (size_t i = NB_TRS_CSE_PASS; i > 0; i--)
    {
      // utils::draw_ir(program, output_filename + std::to_string(i) + ".dot");
      trs.apply_rewrite_rules_on_program_from_static_ruleset();
      cse_pass.apply_cse2();
    }
    std::cout << "TRS et CSE ... termine \n";
    std::cout << std::string(50, '-') << "\n \n";
  }
  else
    std::cout << "optimization is disabled\n";

  // utils::draw_ir(program, output_filename + std::to_string(NB_TRS_CSE_PASS) + ".dot");

  // utils::draw_ir(program, output_filename + "2.dot");

  // be careful, not rewrite rules should applied after calling this pass otherwise you will have to call it again

  std::string sep("-");
  size_t llength = 50;
  for (size_t i = 0; i < llength; i++)
  {
    sep += "-";
  }

  if (ENABLE_OPTIMIZATION)
  {
    fheco_passes::RotationKeySelctionPass rs_pass(program, params);
    rs_pass.collect_program_rotations_steps();
    std::cout << "nombre des etapes de rotation apres la decomposition : " << program->get_rotations_steps().size()
              << "\n";
    std::cout << sep << "\n \n";
  }

  if (ENABLE_OPTIMIZATION)
    cse_pass.apply_cse2();

  std::cout << "insertion de l'operation de relinearization ... \n";
  fheco_passes::RelinPass relin_pass(program);
  relin_pass.simple_relinearize();
  std::cout << sep << "\n \n";

  std::cout << "generation du code ... \n";
  translator::Translator tr(program, params);
  {
    std::ofstream translation_os(output_filename);
    std::ofstream os_header("Computation.hpp");
    std::ofstream os_source("Computation.cpp");
    if (!os_header)
      throw("couldn't open header file for translation.\n");
    if (!os_source)
      throw("couldn't open source file for translation.\n");

    tr.translate_program(10000, os_header, os_source);
    translation_os.close();
  }
  std::cout << sep << "\n \n";

  ir::print_program_depth(program);

  ir::print_ops_counters(program);

  std::cout << sep << "\n \n";

  delete program;
  program = nullptr;
}

} // namespace fhecompiler
