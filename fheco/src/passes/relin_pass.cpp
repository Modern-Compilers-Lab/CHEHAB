#include "relin_pass.hpp"
#include "trs.hpp"

namespace fheco_passes
{

size_t RelinPass::relin_instruction_id = 0;

void RelinPass::simple_relinearize()
{
  /*
    This implements a simple relinearization(relin) strategy in FHE applications, where a relin instruction is
    inserted after each cipher_cipher mutliplication or square
  */

  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);

  fheco_trs::TRS trs(program);

  fheco_trs::MatchingTerm x(fheco_trs::TermType::ciphertextType);
  fheco_trs::MatchingTerm y(fheco_trs::TermType::ciphertextType);
  std::vector<fheco_trs::RewriteRule> relin_rules = {{x * y, fheco_trs::relin(x * y)}};
  trs.apply_trs_rewrite_rules_on_program(relin_rules);
}

} // namespace fheco_passes
