#include"term.hpp"

namespace ir
{

void Term::add_operand(const Ptr& operand) { (* this->operands).push_back(operand); }

bool Term::merge_with_node(Ptr node_to_merge_with_ptr)
{
  if(this->opcode == node_to_merge_with_ptr->get_opcode())
  {
    for( auto& operand_ptr : *(node_to_merge_with_ptr->get_operands()) )
        this->add_operand(operand_ptr);
        
    return true; 
  }
  return false;
}

} // namespace ir
