#include"program.hpp"

namespace ir
{

using Ptr = std::shared_ptr<Term>;

Ptr Program::insert_operation_node(OpCode opcode, const std::vector<Ptr>& operands, std::string label, TermType term_type)
{
  Term::Operation operation(opcode, operands);
  Ptr new_term = std::make_shared<Term>(operation, label);
  new_term->set_term_type(term_type);
  new_term->set_operation_attribute(operation);
  this->data_flow->insert_node(new_term);

  if(opcode == ir::sub)
  {
    std::cout << new_term->get_label() << "(" << new_term->get_output_flag() << ")" << " = " << operands[0]->get_label() << "(" << operands[0]->get_output_flag() << ")" << " - " << operands[1]->get_label() << "(" << operands[1]->get_output_flag() << ")\n";
  }

  if(opcode == ir::assign)
  {
    std::cout << new_term->get_label() << "(" << new_term->get_output_flag() << ")" << " = " << operands[0]->get_label() << "(" << operands[0]->get_output_flag() << ")\n";
  }
  if(opcode == ir::add)
  {
    std::cout << new_term->get_label() << "(" << new_term->get_output_flag() << ")" << " = " << operands[0]->get_label() << "(" << operands[0]->get_output_flag() << ")" << " + " << operands[1]->get_label() << "(" << operands[1]->get_output_flag() << ")\n";
  }
  return new_term;
}

void Program::set_symbol_as_output(std::string symbol)
{
  auto symbol_node_ptr = find_node_in_data_flow(symbol);
  if(symbol_node_ptr ) symbol_node_ptr->set_output_flag(true);
}

Ptr Program::find_node_in_data_flow(std::string label) const
{
   return this->data_flow->find_node(label);
}

void Program::insert_entry_in_constants_table(std::pair<std::string, ConstantTableEntry> table_entry)
{
  this->constants_table.insert(table_entry);
}

bool Program::delete_entry_from_constants_table(std::string entry_key)
{
  //O(logn) complexity, because constants_table is a balanced binary search tree (Red black tree)
  auto it = constants_table.find(entry_key);
  if(it != constants_table.end() )
  {
    constants_table.erase(it); 
    return true;
  }
  else return false;

}

std::optional<std::reference_wrapper<ConstantTableEntry>> Program::get_entry_form_constants_table(const std::string& search_key)
{
  //search key is the label/symbol of the object
  auto it = constants_table.find(search_key);
  if(it != constants_table.end() ) return (*it).second;
  return std::nullopt;
}

bool Program::insert_new_entry_from_existing(std::string new_entry_key, std::string exsisting_entry_key)
{
  auto existing_entry_opt = get_entry_form_constants_table(exsisting_entry_key);

  if(existing_entry_opt == std::nullopt) return false;
  ConstantTableEntry existing_entry = *existing_entry_opt;
  insert_entry_in_constants_table({new_entry_key, existing_entry});
  
  return true;
}

bool Program::insert_new_entry_from_existing_with_delete(std::string new_entry_key, std::string exsisting_entry_key)
{
  
  bool is_inserted = insert_new_entry_from_existing(new_entry_key, exsisting_entry_key);
  if( !is_inserted ) return false;
  delete_entry_from_constants_table(exsisting_entry_key);
  return true;

}


} // namespace ir
