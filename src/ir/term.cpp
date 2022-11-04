#include "term.hpp"
#include <unordered_set>

namespace ir
{

void Term::add_operand(const Ptr &operand)
{
  (*this->operands).push_back(operand);
}

/*

  This function merges a given node that represents a term with another node that represents another term, the other
  node needs to be an operation node excluding assignement and encryption

*/

bool Term::merge_with_node(Ptr node_to_merge_with)
{
  /*
    nullptr edge case
  */
  if (node_to_merge_with == nullptr)
    return false;

  /*
    merge a node with itself edge case
  */
  if (node_to_merge_with->get_label() == this->get_label())
    return false;

  /*
    node to merge with is not an operation node
  */

  if (node_to_merge_with->get_operands() == std::nullopt)
    return false;

  /*
    a constant node cant be merged with any other node
  */
  if (this->operands == std::nullopt)
    return false;

  /*
    node is operation but it represents an assignement or encryption, encoding ..
  */
  const std::unordered_set<ir::OpCode> static_opcodes = {
    ir::OpCode::assign, ir::OpCode::encrypt}; // don't merge with these operations

  ir::OpCode node_opcode = node_to_merge_with->get_opcode();

  if (static_opcodes.find(node_opcode) != static_opcodes.end())
    return false;

  /*
    two nodes must have same operation code, except for case of assign for the node to merge
  */
  if (this->opcode != node_to_merge_with->get_opcode() && this->opcode != ir::OpCode::assign)
    return false;

  /*
    at this point we need to merge, but we first we need to decide how and when we merge also if there is any specific
    data structure is needed for performance
  */

  return true; // everything went well, life is good !
}

} // namespace ir
