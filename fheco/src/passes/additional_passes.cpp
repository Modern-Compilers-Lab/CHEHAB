#include "additional_passes.hpp"

namespace fheco_passes
{

void optimize_SOR_pass(ir::Program *program, ir::OpCode target_opcode)
{
  auto &nodes = program->get_dataflow_sorted_nodes(true);
  // collect sum of rotation nodes
  std::unordered_map<ir::Program::Ptr, ir::Program::Ptr> next;
  std::unordered_map<ir::Program::Ptr, ir::Program::Ptr> head;
  std::unordered_map<ir::Program::Ptr, ir::Program::Ptr> tail;
  std::unordered_set<ir::Program::Ptr> SORs;

  auto save_circuit_condition = [](const ir::Program::Ptr node_ptr) -> bool {
    return node_ptr->get_parents_labels().size() <= 1;
  };

  auto is_combination_of_two_rotations =
    [&target_opcode](const ir::Program::Ptr &node_ptr, bool &only_rotations) -> bool {
    if (node_ptr->get_opcode() != target_opcode)
      return false;
    auto &lhs = node_ptr->get_operands()[0];
    auto &rhs = node_ptr->get_operands()[1];

    only_rotations = (lhs->get_opcode() == ir::OpCode::rotate && rhs->get_opcode() == ir::OpCode::rotate) &&
                     (ir::get_rotation_node_operand(lhs) == ir::get_rotation_node_operand(rhs));

    // something like (x + (x<<p))

    if (rhs->get_opcode() != ir::OpCode::rotate)
      return false;

    return true;
  };

  for (auto &node : nodes)
  {

    head[node] = node;
    tail[node] = node;
    next[node] = nullptr;

    if (node->get_opcode() != target_opcode)
      continue;

    if (node->get_term_type() != ir::TermType::ciphertext)
      continue;

    bool only_rotations(false);

    if (is_combination_of_two_rotations(node, only_rotations))
    {
      if (only_rotations)
        std::cout << "only rotations...\n";
      SORs.insert(node);
      auto &lhs = node->get_operands()[0];
      auto &rhs = node->get_operands()[1];
      if (lhs != rhs)
        next[lhs] = rhs; // to avoid cycles
      head[node] = lhs;
      tail[node] = rhs;
    }
    else
    {
      auto &lhs = node->get_operands()[0];
      auto &rhs = node->get_operands()[1];

      auto lhs_it = SORs.find(lhs);
      auto rhs_it = SORs.find(rhs);

      // we have 3 cases
      if (lhs_it != SORs.end() && rhs_it != SORs.end())
      {
        if (ir::get_rotation_node_operand(tail[*lhs_it]) == ir::get_rotation_node_operand(head[*rhs_it]))
        {
          next[tail[*lhs_it]] = head[*rhs_it];
          head[node] = head[*lhs_it];
          tail[node] = tail[*rhs_it];
          SORs.insert(node);
          SORs.erase(lhs_it);
          SORs.erase(rhs_it);
        }
      }
      else if (lhs_it != SORs.end() && rhs->get_opcode() == ir::OpCode::rotate)
      {
        if (ir::get_rotation_node_operand(tail[*lhs_it]) == ir::get_rotation_node_operand(rhs))
        {
          next[tail[*lhs_it]] = rhs;
          head[node] = head[*lhs_it];
          tail[node] = rhs;
          SORs.erase(lhs_it);
          SORs.insert(node);
        }
      }
      else if (rhs_it != SORs.end() && lhs->get_opcode() == ir::OpCode::rotate) /*this only necessary for add and mul,
                                                                                   and not sub*/
      {
        if (ir::get_rotation_node_operand(lhs) == ir::get_rotation_node_operand(head[*rhs_it]))
        {
          head[node] = lhs;
          tail[node] = tail[*rhs_it];
          next[lhs] = head[*rhs_it];
          SORs.erase(rhs_it);
          SORs.insert(node);
        }
      }
    }
  }

  std::cout << SORs.size() << "\n";

  for (auto &sor_node : SORs)
  {
    // we collect and we perform rewrite
    std::vector<ir::Program::Ptr> rot_nodes;
    auto curr_node = head[sor_node];

    if (curr_node->get_opcode() != ir::OpCode::rotate)
    {
      curr_node = next[curr_node];
    }

    if (curr_node->get_opcode() != ir::OpCode::rotate)
      throw std::logic_error("only the first node is allowed to not to be a rotation node");

    while (curr_node != nullptr)
    {
      rot_nodes.push_back(curr_node);
      curr_node = next[curr_node];
    }

    if (rot_nodes.size() < 3)
      continue;

    auto rot_sort_comp = [](const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs) -> bool {
      int32_t lhs_rot_step = ir::get_rotation_step(lhs);
      int32_t rhs_rot_step = ir::get_rotation_step(rhs);
      return lhs_rot_step < rhs_rot_step;
    };

    sort(rot_nodes.begin(), rot_nodes.end(), rot_sort_comp);
    bool optimize = true;
    int32_t diff = ir::get_rotation_step(rot_nodes[1]) - ir::get_rotation_step(rot_nodes[0]);

    for (size_t i = 0; i < rot_nodes.size(); i++)
    {
      if (
        i + 1 < rot_nodes.size() &&
        (ir::get_rotation_step(rot_nodes[i + 1]) - ir::get_rotation_step(rot_nodes[i]) != diff))
      {
        optimize = false;
        break;
      }
    }

    if (optimize == true)
    {
      auto p_node = optimize_SOR_helper(rot_nodes, program, target_opcode);
      if (p_node != nullptr)
      {
        std::vector<ir::Program::Ptr> operands;
        if (head[sor_node]->get_opcode() != ir::OpCode::rotate)
        {
          operands = {head[sor_node], p_node};
        }
        else
        {
          operands = p_node->get_operands();
        }
        sor_node->clear_operands();
        sor_node->set_operands(operands);
      }
    }
  }
}

void optimize_SOR_pass2(ir::Program *program, ir::OpCode target_opcode)
{

  auto &nodes = program->get_dataflow_sorted_nodes(true);
  std::vector<SORSequence> sor_sequences;

  auto check_adding_node = [](const SORSequence &sor_sequence, const ir::Program::Ptr &node) -> bool {
    if (node->get_opcode() == ir::OpCode::rotate)
      return true;
    return node->get_opcode() == sor_sequence.get_opcode();
  };

  for (auto &node : nodes)
  {
    if (node->get_opcode() == ir::OpCode::undefined)
      continue;

    if (node->get_opcode() != ir::OpCode::mul && node->get_opcode() != ir::OpCode::add)
      continue;

    auto &lhs = node->get_operands()[0];
    auto &rhs = node->get_operands()[1];
    if (sor_sequences.empty() == false && check_adding_node(sor_sequences.back(), node))
    {
      if (lhs->get_opcode() == ir::OpCode::rotate)
        sor_sequences.back().add_rot_node(lhs);
      else if (lhs->get_opcode() == ir::OpCode::undefined)
        sor_sequences.back().add_arithmetic_node_leaves(lhs);

      if (rhs->get_opcode() == ir::OpCode::rotate)
        sor_sequences.back().add_rot_node(rhs);
      else if (rhs->get_opcode() == ir::OpCode::undefined)
        sor_sequences.back().add_arithmetic_node_leaves(rhs);
    }
    else
    {
      SORSequence sor_sequence(node->get_opcode(), node);
      if (lhs->get_opcode() == ir::OpCode::rotate)
        sor_sequence.add_rot_node(lhs);
      else if (lhs->get_opcode() == ir::OpCode::undefined)
        sor_sequence.add_arithmetic_node_leaves(lhs);

      if (rhs->get_opcode() == ir::OpCode::rotate)
        sor_sequence.add_rot_node(rhs);
      else if (rhs->get_opcode() == ir::OpCode::undefined)
        sor_sequence.add_arithmetic_node_leaves(rhs);

      sor_sequences.push_back(sor_sequence);
    }
  }
  // we got all sequences, now we need to select the ones that we want to rewrite

  auto rot_comp = [](const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs) {
    return ir::get_rotation_step(lhs) < ir::get_rotation_step(rhs);
  };
  std::vector<ir::Program::Ptr> nodes_after_processing;
  for (auto &sor_sequence : sor_sequences)
  {
    auto sequence_rotations = sor_sequence.get_rot_nodes();

    if (sequence_rotations.empty() == false)
    {
      sort(sequence_rotations.begin(), sequence_rotations.end(), rot_comp);
      size_t i = 0;
      while (i < sequence_rotations.size())
      {
        if (sequence_rotations.size() - i >= 3)
        {
          std::vector<ir::Program::Ptr> sequence_to_optimize;
          size_t diff = ir::get_rotation_step(sequence_rotations[i + 1]) - ir::get_rotation_step(sequence_rotations[i]);
          sequence_to_optimize.push_back(sequence_rotations[i]);
          sequence_to_optimize.push_back(sequence_rotations[i + 1]);
          std::cout << diff << "\n";

          i += 1;
          while (i + 1 < sequence_rotations.size() && (ir::get_rotation_step(sequence_rotations[i + 1]) -
                                                       ir::get_rotation_step(sequence_rotations[i])) == diff)
          {
            sequence_to_optimize.push_back(sequence_rotations[i + 1]);
            i += 1;
          }
          if (sequence_to_optimize.size() <= 2)
            for (auto &r : sequence_to_optimize)
              nodes_after_processing.push_back(r);
          else
          {
            nodes_after_processing.push_back(
              optimize_SOR_helper(sequence_to_optimize, program, sor_sequence.get_opcode()));
          }
        }
        else
          break;
      }
      while (i < sequence_rotations.size())
      {
        nodes_after_processing.push_back(sequence_rotations[i++]);
      }
    }
    for (auto &r : sor_sequence.get_arithmetic_leaves())
      nodes_after_processing.push_back(r);

    auto new_sor_node = recover_binary_shape_of_SOR(program, nodes_after_processing, sor_sequence.get_opcode());
    sor_sequence.get_root()->clear_operands();
    sor_sequence.get_root()->set_operands(new_sor_node->get_operands());
  }
}

ir::Program::Ptr optimize_SOR_helper(
  std::vector<ir::Program::Ptr> &rot_nodes, ir::Program *program, ir::OpCode target_opcode)
{
  std::vector<ir::Program::Ptr> tree_nodes;

  size_t n = rot_nodes.size();
  auto is_a_power_of_2 = [](size_t n) -> bool {
    return __builtin_popcount(n) == 1;
  };

  while (rot_nodes.size() > 2)
  {
    // each iteration common factor is the median
    n = rot_nodes.size();
    std::vector<ir::Program::Ptr> new_rot_nodes;
    int64_t mid = (n >> 1);

    int64_t left_pointer = 0;
    int64_t right_pointer = ((n % 2 == 1) ? mid + 1 : mid);

    auto mid_node = rot_nodes[mid];

    while (right_pointer < n)
    {
      auto lhs_rot_node = rot_nodes[left_pointer++];
      auto rhs_rot_node = rot_nodes[right_pointer++];
      auto rhs_rot_node_operand = ir::get_rotation_node_operand(rhs_rot_node);
      auto new_t_op_node = program->insert_operation_node_in_dataflow(
        target_opcode, {mid_node, rhs_rot_node_operand}, "", ir::TermType::ciphertext);
      auto lhs_step_node = ir::get_rotation_step_node(lhs_rot_node);
      auto new_rot_node = program->insert_operation_node_in_dataflow(
        ir::OpCode::rotate, {new_t_op_node, lhs_step_node}, "", ir::TermType::ciphertext);

      new_rot_nodes.push_back(new_rot_node);
    }
    tree_nodes.push_back(mid_node);

    while (left_pointer < mid)
      tree_nodes.push_back(rot_nodes[left_pointer++]);

    rot_nodes = new_rot_nodes;
  }
  if (rot_nodes.size() <= 2)
  {
    for (auto &rot_node : rot_nodes)
      tree_nodes.push_back(rot_node);
    std::cout << "tree node size : " << tree_nodes.size() << "\n";
    return recover_binary_shape_of_SOR(program, tree_nodes, target_opcode);
  }
  else
  {
    throw std::logic_error("expected size of rot_nodes is 2 or 1");
  }
}

ir::Program::Ptr recover_binary_shape_of_SOR(
  ir::Program *program, const std::vector<ir::Program::Ptr> &sor, ir::OpCode target_opcode)
{

  /*
  if (sor.size() == 1)
    return sor[0];

  auto curr_node =
    program->insert_operation_node_in_dataflow(target_opcode, {sor[0], sor[1]}, "", ir::TermType::ciphertext);

  for (size_t i = 2; i < sor.size(); i++)
  {
    curr_node =
      program->insert_operation_node_in_dataflow(target_opcode, {curr_node, sor[i]}, "", ir::TermType::ciphertext);
  }

  return curr_node;
  */

  // build balanced binary tree

  size_t n = sor.size();

  if (n == 1)
    return sor[0];

  std::vector<ir::Program::Ptr> curr_level = sor;
  while (curr_level.size() > 1)
  {
    std::vector<ir::Program::Ptr> new_level;
    for (size_t i = 0; i < curr_level.size(); i += 2)
    {
      if (i + 1 < curr_level.size())
      {
        auto lhs = curr_level[i];
        auto rhs = curr_level[i + 1];
        new_level.push_back(
          program->insert_operation_node_in_dataflow(target_opcode, {lhs, rhs}, "", ir::TermType::ciphertext));
      }
      else
        new_level.push_back(curr_level[i]);
    }
    curr_level = new_level;
  }
  return curr_level[0];
}

void optimize_SOR(const ir::Program::Ptr &node, ir::OpCode target_opcode) {}

} // namespace fheco_passes
