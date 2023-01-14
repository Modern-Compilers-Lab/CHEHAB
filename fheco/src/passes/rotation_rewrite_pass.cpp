#include "rotation_rewrite_pass.hpp"
#include "passes_const.hpp"

namespace fheco_passes
{
size_t RotationRewritePass::pass_instruction_id = 0;

void RotationRewritePass::apply_rule1(const std::shared_ptr<ir::Term> &node_term)
{
  // (c << n) + (c << m) = ((c << max(n,m)-min(n,m)) + (c)) << min(n,m)
  // This rule applies on ciphertext type

  auto term_operands = node_term->get_operands();
  if (term_operands.size() != 2)
    return;

  if (rotation_intruction_set.find(term_operands[0]->get_opcode()) == rotation_intruction_set.end())
    return;

  if (rotation_intruction_set.find(term_operands[1]->get_opcode()) == rotation_intruction_set.end())
    return;

  // rotation steps for term_operands[0]
  int32_t n;
  std::shared_ptr<ir::Term> lhs_rotation_operand;

  if (term_operands[0]->get_operands()[0]->get_term_type() == ir::rawDataType)
  {
    n = std::stoi(term_operands[0]->get_operands()[0]->get_label());
    lhs_rotation_operand = term_operands[0]->get_operands()[1];
  }

  else if (term_operands[0]->get_operands()[1]->get_term_type() == ir::rawDataType)
  {
    n = std::stoi(term_operands[0]->get_operands()[1]->get_label());
    lhs_rotation_operand = term_operands[0]->get_operands()[0];
  }

  else
    throw("one of rotation's operand must be a raw data type");

  // rotation steps for term_operands[1]
  int32_t m;
  std::shared_ptr<ir::Term> rhs_rotation_operand;
  if (term_operands[1]->get_operands()[0]->get_term_type() == ir::rawDataType)
  {
    m = std::stoi(term_operands[1]->get_operands()[0]->get_label());
    rhs_rotation_operand = term_operands[1]->get_operands()[1];
  }

  else if (term_operands[1]->get_operands()[1]->get_term_type() == ir::rawDataType)
  {
    m = std::stoi(term_operands[1]->get_operands()[1]->get_label());
    rhs_rotation_operand = term_operands[1]->get_operands()[0];
  }

  else
    throw("one of rotation's operand must be a raw data type");

  ir::Program::Ptr node_to_rotate_with_new_steps;
  if (n < m)
    node_to_rotate_with_new_steps = rhs_rotation_operand;
  else
    node_to_rotate_with_new_steps = lhs_rotation_operand;

  std::string m_str = std::to_string(std::min(n, m));
  ir::Program::Ptr new_rhs = std::make_shared<ir::Term>(m_str, ir::rawDataType);

  ir::OpCode old_opcode = node_term->get_opcode();

  node_term->set_opcode(
    node_term->get_operands()[0]
      ->get_opcode()); // here it doesn't matter whether it is node_term->get_operands()[0]->get_opcode() or
                       // node_term->get_operands()[1]->get_opcode()
  node_term->clear_operands();
  std::string rotation_node_in_lhs_label = inst_keyword + "_" + std::to_string(pass_instruction_id++);
  ir::Program::Ptr steps_raw_node = std::make_shared<ir::Term>(std::to_string(std::abs(n - m)), ir::rawDataType);
  ir::Program::Ptr rotation_node_in_lhs = program->insert_operation_node_in_dataflow(
    node_term->get_opcode(), std::vector<ir::Program::Ptr>({node_to_rotate_with_new_steps, steps_raw_node}),
    rotation_node_in_lhs_label, ir::ciphertextType);

  std::string lhs_label = inst_keyword + "_" + std::to_string(pass_instruction_id++);

  ir::Program::Ptr new_lhs;

  if (n == m)
  {
    new_lhs = program->insert_operation_node_in_dataflow(
      old_opcode, std::vector<ir::Program::Ptr>({lhs_rotation_operand, rhs_rotation_operand}), lhs_label,
      ir::ciphertextType);
  }
  else
  {
    new_lhs = program->insert_operation_node_in_dataflow(
      old_opcode,
      std::vector<ir::Program::Ptr>(
        {rotation_node_in_lhs,
         (node_to_rotate_with_new_steps == lhs_rotation_operand ? rhs_rotation_operand : lhs_rotation_operand)}),
      lhs_label, ir::ciphertextType);
  }

  node_term->add_operand(new_lhs);
  node_term->add_operand(new_rhs);
}

void RotationRewritePass::apply_rule2(const std::shared_ptr<ir::Term> &node_term)
{
  // this rule simplifies successive rotations
  // (ct << n) << m = ct << n+m

  if (node_term->is_operation_node() == false)
    return;

  if (rotation_intruction_set.find(node_term->get_opcode()) == rotation_intruction_set.end())
    return;

  if (node_term->get_operands()[0]->get_term_type() == ir::rawDataType)
    node_term->reverse_operands();

  int32_t rotation_step = std::stoi(node_term->get_operands()[1]->get_label());

  auto operand_to_rotate = node_term->get_operands()[0];

  if (operand_to_rotate->get_opcode() == node_term->get_opcode())
  {
    if (operand_to_rotate->get_operands()[0]->get_term_type() == ir::rawDataType)
      operand_to_rotate->reverse_operands();

    int32_t lhs_rotation_step = std::stoi(operand_to_rotate->get_operands()[1]->get_label());
    std::string new_rotation_step = std::to_string(lhs_rotation_step + rotation_step);
    node_term->get_operands()[1]->set_label(new_rotation_step);
    auto new_operand_to_rotate = operand_to_rotate->get_operands()[0];
    node_term->delete_operand_at_index(0);
    node_term->add_operand(new_operand_to_rotate);
    if (node_term->get_operands()[0]->get_term_type() == ir::rawDataType)
      node_term->reverse_operands(); // in order to put the rawDataType node as the rhs
  }
}
void RotationRewritePass::apply_rule3(const std::shared_ptr<ir::Term> &node_term) {}

void RotationRewritePass::apply_rewrite()
{
  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);

  for (auto node_ptr : sorted_nodes)
  {
    apply_rule2(node_ptr);
    apply_rule1(node_ptr);
    // rule3
    // rule4
  }
}

} // namespace fheco_passes
