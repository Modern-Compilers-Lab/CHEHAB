#include "program.hpp"
#include <unordered_set>
namespace ir
{

using Ptr = std::shared_ptr<Term>;

Ptr Program::insert_operation_node_in_dataflow(
  OpCode opcode, const std::vector<Ptr> &operands, std::string label, TermType term_type)
{
  Ptr new_term = std::make_shared<Term>(opcode, operands, label);
  new_term->set_term_type(term_type);

  for (auto &operand : operands)
    operand->insert_parent_label(label);

  this->data_flow->insert_node(new_term, this->type_of(new_term->get_label()) == ConstantTableEntryType::output);
  return new_term;
}

/* This function deletes only if it is posssible */

void Program::set_node_operands(const std::string &node_label, const std::vector<Ptr> &new_operands)
{
  auto node_ptr = find_node_in_dataflow(node_label);
  if (node_ptr == nullptr)
    return;

  node_ptr->set_operands(new_operands);
}

Ptr Program::find_node_in_dataflow(const std::string &label) const
{
  return this->data_flow->find_node(label);
}

void Program::delete_node_from_dataflow(const std::string &node_label)
{
  data_flow->delete_node(node_label);
}

void Program::insert_entry_in_constants_table(std::pair<std::string, ConstantTableEntry> table_entry)
{
  // if the entry already exists in the table then it will make an update, otherwise it inserts
  auto it = this->constants_table.find(table_entry.first);

  if (it == this->constants_table.end())
  {
    this->constants_table.insert(table_entry);
  }
  else
  {
    it->second = table_entry.second;
  }
}

void Program::delete_node_from_outputs(const std::string &key)
{
  this->data_flow->delete_node_from_outputs(key);
}

bool Program::delete_entry_from_constants_table(std::string entry_key)
{
  // O(1) in average (it depends on hash collisions)
  auto it = constants_table.find(entry_key);
  if (it != constants_table.end())
  {
    constants_table.erase(it);
    return true;
  }
  else
    return false;
}

std::optional<std::reference_wrapper<ConstantTableEntry>> Program::get_entry_form_constants_table(
  const std::string &search_key)
{
  // search key is the label/symbol of the object
  auto it = constants_table.find(search_key);
  if (it != constants_table.end())
    return (*it).second;
  return std::nullopt;
}

bool Program::insert_new_entry_from_existing(std::string new_entry_key, std::string exsisting_entry_key)
{
  auto existing_entry_opt = get_entry_form_constants_table(exsisting_entry_key);

  if (existing_entry_opt == std::nullopt)
    return false;
  ConstantTableEntry existing_entry = *existing_entry_opt;
  insert_entry_in_constants_table({new_entry_key, existing_entry});

  return true;
}

bool Program::insert_new_entry_from_existing_with_delete(std::string new_entry_key, std::string exsisting_entry_key)
{

  bool is_inserted = insert_new_entry_from_existing(new_entry_key, exsisting_entry_key);

  if (!is_inserted)
    return false;

  delete_entry_from_constants_table(exsisting_entry_key);
  return true;
}

ConstantTableEntryType Program::type_of(const std::string &label)
{

  auto table_entry = this->get_entry_form_constants_table(label);
  if (table_entry == std::nullopt)
    return ConstantTableEntryType::undefined;
  else
    return (*table_entry).get().entry_type;
}

const std::unordered_map<std::string, Ptr> &Program::get_outputs_nodes() const
{
  return this->data_flow->get_outputs_nodes();
}

const std::vector<Ptr> &Program::get_dataflow_sorted_nodes(bool clear_existing_order) const
{
  this->data_flow->apply_topological_sort(clear_existing_order);
  return this->data_flow->get_outputs_nodes_topsorted();
}

} // namespace ir
