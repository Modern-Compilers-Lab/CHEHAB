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

std::string Program::get_tag_value_in_constants_table_entry(const std::string &entry_key)
{
  auto table_entry = get_entry_form_constants_table(entry_key);
  if (table_entry == std::nullopt)
  {
    throw("no table entry with the given key in function get_tag_value_in_constants_table_entry()");
  }
  return (*table_entry).get().get_entry_value().get_tag();
}

std::optional<std::string> Program::get_tag_value_in_constants_table_entry_if_exists(const std::string &entry_key)
{
  auto table_entry = get_entry_form_constants_table(entry_key);

  if (table_entry == std::nullopt)
    return std::nullopt;

  return (*table_entry).get().get_entry_value().get_tag();
}

void Program::update_tag_value_in_constants_table_entry(const std::string &entry_key, const std::string &tag_value)
{
  auto table_entry = this->get_entry_form_constants_table(entry_key);
  if (table_entry == std::nullopt)
  {
    insert_entry_in_constants_table({entry_key, {type_of(entry_key), tag_value}});
  }
  else
  {
    (*table_entry).get().set_entry_tag(tag_value);
  }
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

/*
  This a function that applies a simple transformation in order to compact assignement. For example if we have a = b and
  b = c it will transform it to a = c while taking in consideration tags (variable names). This function was moved from
  Translator class to here but later maybe it will be moved to another class that encompass various transformations.
*/
void Program::compact_assignement(const ir::Term::Ptr &node_ptr)
{
  if (!node_ptr->is_operation_node())
    return;

  if (node_ptr->get_opcode() != ir::OpCode::assign)
    return;

  auto operand = node_ptr->get_operands()[0];

  while (operand->is_operation_node() && operand->get_opcode() == ir::OpCode::assign)
  {
    operand = operand->get_operands()[0];
  }
  if (operand->is_operation_node())
  {
    node_ptr->clear_operands();
    node_ptr->set_operands(operand->get_operands());
    node_ptr->set_opcode(operand->get_opcode());
  }
  else
  {
    node_ptr->replace_with(operand);
  }
}

void Program::flatten_term_operand_by_one_level_at_index(const ir::Program::Ptr &node_term, size_t index)
{
  /*
    t2 = a+b
    t1 = t2 + t3 -> t1 = a+b+t3
  */

  /*
    This function is assumed to be run in a bottom up traversal as it doesn't call itself recursively
  */

  if (node_term->is_operation_node() == false)
    return;

  auto flattening_condition = [this](const ir::Program::Ptr &operand) -> bool {
    if (operand->is_operation_node() == false)
      return false;
    if (type_of(operand->get_label()) == ir::ConstantTableEntryType::output)
      return false;
    if (operand->get_parents_labels().size() > 1)
      return false;

    return true;
  };

  if (node_term->get_operands().size() >= index || index < 0)
    return;

  auto operand = node_term->get_operands()[index];

  // term->clear_operands();

  if (flattening_condition(operand) == true)
  {
    node_term->delete_operand_at_index(index);

    for (auto &sub_operand : operand->get_operands())
      node_term->add_operand(sub_operand);
  }
}

bool Program::is_tracked_object(const std::string &label)
{
  auto table_entry = get_entry_form_constants_table(label);

  if (table_entry == std::nullopt)
    return false;

  ir::ConstantTableEntry lhs_table_entry_value = *table_entry;

  ir::ConstantTableEntry::EntryValue entry_value = lhs_table_entry_value.get_entry_value();

  return entry_value.get_tag().length() > 0;
}

} // namespace ir
