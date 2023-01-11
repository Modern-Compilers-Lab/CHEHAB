#include "rotationkeys_select_pass.hpp"

namespace fheco_passes
{

std::vector<int> get_unique_rotation_steps(ir::Program *program)
{

  std::unordered_set<int> rotation_steps;
  /* rotate_columns is not included in rotation_intruction_set since this instruction doesn't take steps as input */
  std::unordered_set<ir::OpCode> rotation_intruction_set = {ir::OpCode::rotate, ir::OpCode::rotate_rows};

  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node : sorted_nodes)
  {
    if (node->is_operation_node() == false)
      continue;

    if (rotation_intruction_set.find(node->get_opcode()) == rotation_intruction_set.end())
      continue;

    auto &operands = node->get_operands();
    std::string steps_raw_data;
    if (operands.size() != 2)
      throw("rotation instruction with more than 2 operands in get_unique_rotation_steps function");

    if (operands[0]->get_term_type() == ir::rawDataType)
    {
      steps_raw_data = operands[0]->get_label();
    }
    else if (operands[1]->get_term_type() == ir::rawDataType)
    {
      steps_raw_data = operands[1]->get_label();
    }
    else
      throw("one of rotation operands must be a rawDataType in get_unique_rotation_steps function");

    rotation_steps.insert(std::stoi(steps_raw_data));
  }
  std::vector<int> rotations_steps_vec;
  rotations_steps_vec.insert(rotations_steps_vec.end(), rotation_steps.begin(), rotation_steps.end()); // C++17

  return rotations_steps_vec;
}

} // namespace fheco_passes
