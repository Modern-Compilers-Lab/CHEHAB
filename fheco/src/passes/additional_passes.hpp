#pragma once
#include "ir_utils.hpp"
#include "program.hpp"

namespace fheco_passes
{

/*
  optimize sum of rotations
*/

class SORSequence
{
private:
  ir::OpCode opcode;
  std::vector<ir::Program::Ptr> leaves; // leaves from arithmetic nodes (mul and add)
  std::vector<ir::Program::Ptr> rot_nodes;
  ir::Program::Ptr root;

public:
  SORSequence(ir::OpCode s_opcode) : opcode(s_opcode) {}
  SORSequence(ir::OpCode s_opcode, const ir::Program::Ptr &_root) : opcode(s_opcode), root(_root) {}

  void add_rot_node(const ir::Program::Ptr &node)
  {
    if (node->get_opcode() != ir::OpCode::rotate)
      throw std::logic_error("rotation node expected in add_rot_node");
    rot_nodes.push_back(node);
  }

  void add_arithmetic_node_leaves(const ir::Program::Ptr &node)
  {
    if (node->get_opcode() != ir::OpCode::undefined)
      throw std::logic_error("only leaf nodes are expected in add_arithmetic_node_leaves");
    leaves.push_back(node);
  }

  ir::Program::Ptr get_root() { return root; }

  void set_root(const ir::Program::Ptr &_root) { root = _root; }

  const std::vector<ir::Program::Ptr> &get_arithmetic_leaves() const { return leaves; }

  const std::vector<ir::Program::Ptr> &get_rot_nodes() const { return rot_nodes; }

  ir::OpCode get_opcode() const { return opcode; }
};

void optimize_SOR_pass(ir::Program *program, ir::OpCode target_opcode = ir::OpCode::add);

void optimize_SOR(const ir::Program::Ptr &node, ir::OpCode target_opcode = ir::OpCode::add);

ir::Program::Ptr optimize_SOR_helper(
  std::vector<ir::Program::Ptr> &rot_nodes, ir::Program *program, ir::OpCode target_opcode);

ir::Program::Ptr recover_binary_shape_of_SOR(
  ir::Program *program, const std::vector<ir::Program::Ptr> &sor, ir::OpCode target_opcode);

void optimize_SOR_pass2(ir::Program *program, ir::OpCode target_opcode);

} // namespace fheco_passes
