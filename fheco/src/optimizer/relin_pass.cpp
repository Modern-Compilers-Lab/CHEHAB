#include "relin_pass.hpp"
#include "program.hpp"

namespace optimizer
{

size_t RelinPass::relin_instruction_id = 0;

void RelinPass::simple_relinearize()
{
  /*
    This implements a simple relinearization(relin) strategy in FHE applications, where a relin instruction is
    inserted after each cipher_cipher mutliplication or square
  */
  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);

  for (auto &node_ptr : sorted_nodes)
  {

    if (!node_ptr->is_operation_node())
      continue;

    ir::TermType term_type = node_ptr->get_term_type();

    if (term_type != ir::ciphertextType)
      continue;

    ir::OpCode term_opcode = node_ptr->get_opcode();

    if (term_opcode != ir::OpCode::mul && term_opcode != ir::OpCode::square)
      continue;

    /*rewrite, where rule is a * b -> relinearize(a*b) . a and b are ciphertexts */

    std::string relin_inst_tag = node_ptr->get_label() + inst_keyword + std::to_string(relin_instruction_id++);
    ir::Term copy_term(*(node_ptr.get()));
    copy_term.clear_parents();
    std::shared_ptr<ir::Term> copy_ptr = std::make_shared<ir::Term>(copy_term);
    std::vector<std::shared_ptr<ir::Term>> relin_operands = {copy_ptr};
    auto relin_node_ptr = std::make_shared<ir::Term>(ir::OpCode::relinearize, relin_operands, relin_inst_tag);
    relin_node_ptr->set_term_type(ir::ciphertextType);
    node_ptr->replace_with(relin_node_ptr);
  }
}

} // namespace optimizer
