#include"program.hpp"

namespace ir
{

using Ptr = std::shared_ptr<Term>;

Ptr Program::insert_operation_node_in_dataflow(OpCode opcode, const std::vector<Ptr>& operands, std::string label, TermType term_type)
{
  Ptr new_term = std::make_shared<Term>(opcode, operands, label);
  new_term->set_term_type(term_type);
  this->data_flow->insert_node(new_term);
  return new_term;
}

void Program::set_symbol_as_output(const std::string& label, const std::string& tag)
{

  //set as output in data flow (DAG, IR)
  auto symbol_node_ptr = find_node_in_dataflow(label);
  if(symbol_node_ptr ) symbol_node_ptr->set_output_flag(true);

  //set as output in ConstantsTable
  auto constant_table_entry = this->get_entry_form_constants_table(label);
  if(constant_table_entry == std::nullopt)
  {
    std::string tag_to_insert = tag.length() ? tag : (label+"_"+datatype::output_tag);
    this->insert_entry_in_constants_table({label, {ir::ConstantTableEntry::output, {tag_to_insert}}});
  }
  else
  {
    ir::ConstantTableEntry& table_entry = *constant_table_entry;
    table_entry.set_entry_type(ir::ConstantTableEntry::output);
    table_entry.set_entry_tag(tag);
  }

}

Ptr Program::find_node_in_dataflow(const std::string& label) const
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

void Program::traverse_dataflow()
{
  this->data_flow->traverse();
}

} // namespace ir
