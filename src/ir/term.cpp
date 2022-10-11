#include"term.hpp"

namespace ir
{

bool Term::merge_with_node(Ptr node_to_merge_with)
{
  if(this->operation != std::nullopt && node_to_merge_with->get_operation_attribute() != std::nullopt)
  {
    Operation node_to_merge_with_operation = *(node_to_merge_with->get_operation_attribute());
    Operation this_operation = *(this->get_operation_attribute());
    if(this_operation.get_opcode() == node_to_merge_with_operation.get_opcode()) 
    {
      for( auto& operand_ptr : node_to_merge_with_operation.get_operands() )
        this_operation.add_operand(operand_ptr);

      return true;
    } 
    else return false;    
  }
  return false;
}

} // namespace ir
