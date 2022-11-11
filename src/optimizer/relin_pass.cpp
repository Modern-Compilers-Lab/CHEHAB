#include "relin_pass.hpp"
#include "program.hpp"

namespace optimizer
{

size_t RelinPass::relin_instruction_id = 0;

void RelinPass::simple_relinearize()
{
  /*
    This implements the simplest relinearization strategy in FHE applications, where a relin instruction is inserted
    after each cipher_cipher mutliplication or square
  */
  auto &sorted_nodes = program->get_dataflow_sorted_nodes();
  for (auto &node_ptr : sorted_nodes)
  {

    if (node_ptr->get_operands() == std::nullopt)
      continue;

    ir::TermType term_type = node_ptr->get_term_type();

    if (term_type != ir::ciphertextType)
      continue;

    ir::OpCode term_opcode = node_ptr->get_opcode();

    if (term_opcode != ir::OpCode::mul && term_opcode != ir::OpCode::square)
      continue;

    /*rewrite, where rule is a * b -> relinearize(a*b) . a and b are ciphertexts */
    std::string relin_inst_tag = node_ptr->get_label() + inst_keyword + std::to_string(relin_instruction_id++);
    ir::Term current_term_copy = *node_ptr.get(); /* call copy constructor */
    std::shared_ptr<ir::Term> copy_term = std::make_shared<ir::Term>(current_term_copy);
    ir::Term new_term(ir::OpCode::relinearize, {copy_term}, relin_inst_tag);
    new_term.set_term_type(ir::ciphertextType);
    (*node_ptr.get()).replace_with(new_term);
  }
}

} // namespace optimizer
