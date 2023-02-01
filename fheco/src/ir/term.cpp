#include "term.hpp"
#include <unordered_set>

namespace ir
{

size_t Term::term_id = 0;

void Term::replace_operand_with(const Ptr &operand, const Ptr &to_replace_with)
{
  if (is_operation_node() == false)
    return;
  for (size_t i = 0; i < get_operands().size(); i++)
  {
    if (get_operands()[i] == operand)
    {
      set_operand_at_index(i, to_replace_with);
      break;
    }
  }
}

void Term::delete_operand_term(const std::string &term_label)
{
  // this needs to be changed later .. since it is unecessary linear in terms of complexity
  size_t pos = -1;
  for (size_t i = 0; i < operation_attribute->operands.size(); i++)
  {
    if (operation_attribute->operands[i]->get_label() == term_label)
    {
      pos = i;
      break;
    }
  }

  if (pos >= 0)
  {
    size_t degree = operation_attribute->operands.size();
    operation_attribute->operands[pos]->delete_parent(this->label);
    // operation_attribute->operands.erase(operation_attribute->operands.begin() + pos);
    operation_attribute->operands[pos] = operation_attribute->operands.back();
    operation_attribute->operands.pop_back();
  }
}

void Term::insert_parent_label(const std::string &label)
{
  parents_labels.insert(label);
}

void Term::add_operand(const Ptr &operand)
{
  operand->add_parent_label(this->label);
  (*operation_attribute).operands.push_back(operand);
}

/*

  This function merges a given node that represents a term with another node that represents another term, the other
  node needs to be an operation node excluding assignement and encryption

*/

void Term::delete_parent(const std::string &parent_label)
{
  auto it = parents_labels.find(parent_label);
  if (it == parents_labels.end())
    return;
  parents_labels.erase(it);
}

void Term::add_parent_label(const std::string &parent_label)
{
  parents_labels.insert(parent_label);
}

void Term::set_operand_at_index(size_t index, const Ptr &operand_ptr)
{
  if (is_operation_node() == false)
    return;

  if (index < 0)
    return;

  if (index >= get_operands().size())
  {
    add_operand(operand_ptr);
    return;
  }
  operation_attribute->operands[index]->delete_parent(this->get_label());
  operation_attribute->operands[index] = operand_ptr;
  operand_ptr->add_parent_label(this->label);
}

void Term::delete_operand_at_index(size_t index)
{
  if (!is_operation_node())
    return;

  if (index < 0)
    return;

  if (index >= operation_attribute->operands.size())
    return;

  if (operation_attribute->operands.size() == 1)
  {
    operation_attribute->operands[0]->delete_parent(this->label);
    operation_attribute->operands.pop_back();
    return;
  }

  size_t degree = operation_attribute->operands.size();
  operation_attribute->operands[index]->delete_parent(this->label);
  operation_attribute->operands[index] = operation_attribute->operands.back();
  // operation_attribute->operands.erase(operation_attribute->operands.begin() + pos);
  operation_attribute->operands.pop_back();
}

void Term::sort_operands(std::function<bool(const Ptr &, const Ptr &)> comp)
{
  if (!is_operation_node())
    return;

  sort(operation_attribute->operands.begin(), operation_attribute->operands.end(), comp);
}

void Term::clear_operands()
{
  if (!is_operation_node())
    return;

  while (operation_attribute->operands.size() > 0)
  {
    delete_operand_at_index(operation_attribute->operands.size() - 1);
  }
}

void Term::replace_with(const Ptr &rhs)
{
  (*(rhs.get())).parents_labels = (*this).parents_labels;
  *this = *(rhs.get());
}

void Term::set_a_default_label()
{
  label = term_type_str[type] + std::to_string(term_id);
}

} // namespace ir
