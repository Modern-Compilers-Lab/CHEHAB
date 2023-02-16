#include "normalize_pass.hpp"
#include "draw_ir.hpp"
#include "ir_utils.hpp"
#include "passes_const.hpp"
#include <algorithm>

namespace fheco_passes
{

void Normalizer::flatten_by_one_level(const ir::Program::Ptr &node)
{
  if (node->is_operation_node() == false)
    return;

  if (target_arithmetic_opcodes.find(node->get_opcode()) == target_arithmetic_opcodes.end())
    return;

  auto flatten_condition = [this](const ir::Program::Ptr &parent_node, const ir::Program::Ptr &node) {
    bool flexible_condition = node->get_parents_labels().size() <= 1; // This can be changed later

    return (parent_node->get_opcode() == node->get_opcode()) && flexible_condition &&
           (this->consolidated_exprs.find(node) == this->consolidated_exprs.end());
  };

  std::vector<ir::Program::Ptr> operands = node->get_operands();
  node->clear_operands();

  for (size_t i = 0; i < operands.size(); i++)
  {
    if (flatten_condition(node, operands[i]))
    {
      for (auto &deep_operand : operands[i]->get_operands())
        node->add_operand(deep_operand);
    }
    else
      node->add_operand(operands[i]);
  }
}

void Normalizer::normalize_rotations(const ir::Program::Ptr &node)
{
  // we care only about rotations
  if (target_arithmetic_opcodes.find(node->get_opcode()) == target_arithmetic_opcodes.end())
    return;

  std::vector<ir::Program::Ptr> rotation_nodes;
  std::vector<ir::Program::Ptr> non_rotation_nodes;
  /*
    This step is to exclude first rotation element for normalization process or not as the latter could
    lead to re-arrangement of nodes which could lead by itself to wrong results in case of subtraction
  */
  for (int32_t i = node->get_operands().size() - 1; i >= 0; i--)
  {
    auto operand = node->get_operands()[i];

    if (node->get_opcode() == ir::OpCode::sub && i == 0)
      continue;

    else if (rotation_intruction_set.find(operand->get_opcode()) != rotation_intruction_set.end())
    {
      rotation_nodes.push_back(operand);
      node->delete_operand_at_index(i);
    }
  }
  // node->clear_operands();
  /*
    we define an order for rotation nodes
  */
  auto rot_comp = [](const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs) {
    return ir::get_rotation_step(lhs) < ir::get_rotation_step(rhs);
  };

  sort(rotation_nodes.begin(), rotation_nodes.end(), rot_comp);
  // for (auto &other_node : non_rotation_nodes)
  // node->add_operand(other_node);

  for (auto rot_node : rotation_nodes)
    node->add_operand(rot_node);
}

void Normalizer::normalize()
{
  {
    auto &nodes = program->get_dataflow_sorted_nodes(true);
    /*
      We remove associativity and then we sort
    */
    for (auto &node : nodes)
    {
      if (node->is_operation_node() == false)
        continue;

      flatten_by_one_level(node);
    }
  }

  auto &nodes = program->get_dataflow_sorted_nodes(true);
  for (auto node : nodes)
  {
    // normalize_rotations(node);
    if (node->is_operation_node())
    {
      normalize_rotations(node);
      recover_binary_form(node);
    }
  }
}

void Normalizer::recover_binary_form(ir::Program::Ptr &node)
{
  if (node->is_operation_node() == false)
    return;

  if (node->get_operands().size() <= 2)
    return;

  /*
    we split in the middle naively, anything related to depth optimization should be handled by TRS, thus this function
    is supposed to be called before TRS
  */
  auto operands = node->get_operands();
  auto node_binary_copy = recover_binary_form_helper(node, operands, 0, operands.size() - 1);
  node->clear_operands();
  node->set_operands(node_binary_copy->get_operands());
}

ir::Program::Ptr Normalizer::recover_binary_form_helper(
  const ir::Program::Ptr &origin_node, const std::vector<ir::Program::Ptr> &operands, size_t start_index,
  size_t end_index)
{
  if (start_index == end_index)
    return operands[start_index];

  size_t mid = start_index + ((end_index - start_index) >> 1);

  ir::Program::Ptr lhs = recover_binary_form_helper(origin_node, operands, start_index, mid);
  ir::Program::Ptr rhs = recover_binary_form_helper(origin_node, operands, mid + 1, end_index);

  ir::Program::Ptr new_op_node = program->insert_operation_node_in_dataflow(
    origin_node->get_opcode(), std::vector<ir::Program::Ptr>({lhs, rhs}), "", ir::deduce_ir_term_type(lhs, rhs));

  return new_op_node;
}

} // namespace fheco_passes
