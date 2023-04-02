#include "dag.hpp"
#include <algorithm>
#include <optional>
#include <stack>
#include <stdexcept>
#include <unordered_set>

namespace ir
{
using Ptr = std::shared_ptr<Term>;

void DAG::insert_node(Ptr node, bool is_output)
{
  if (is_output)
    outputs_nodes.insert({node->get_label(), node});

  this->node_ptr_from_label[node->get_label()] = node;
}

Ptr DAG::find_node(const std::string &node_label) const
{
  auto it = node_ptr_from_label.find(node_label);
  if (it != node_ptr_from_label.end())
    return (*it).second;
  return nullptr;
}

void DAG::delete_node(const std::string &node_label)
{
  auto it = node_ptr_from_label.find(node_label);

  if (it == node_ptr_from_label.end())
    return;

  if (it->second->is_operation_node())
  {
    for (auto &operand : it->second->get_operands())
    {
      operand->delete_parent(node_label);
    }
  }

  for (auto &parent_label : it->second->get_parents_labels())
  {
    auto it_parent = node_ptr_from_label.find(parent_label);
    if (it_parent != node_ptr_from_label.end())
    {
      it_parent->second->delete_operand_term(node_label);
    }
  }
  node_ptr_from_label.erase(it);
}

void DAG::set_node_as_output(const std::string &key)
{
  auto node = find_node(key);
  if (!node)
    throw std::invalid_argument("node not found");

  outputs_nodes.insert({key, node});
}

void DAG::unset_node_from_output(const std::string &key)
{
  auto it = this->outputs_nodes.find(key);
  if (it != this->outputs_nodes.end())
  {
    this->outputs_nodes.erase(it);
  }
}

void DAG::apply_topological_sort(bool clear_existing_order)
{
  if (sorted_nodes.size() && clear_existing_order == false)
    return;

  sorted_nodes.clear();
  std::stack<std::pair<bool, Ptr>> traversal_stack;
  std::unordered_set<Ptr> visited_nodes;
  for (auto &e : outputs_nodes)
  {
    auto &node_ptr = e.second;

    if (visited_nodes.find(node_ptr) == visited_nodes.end())
    {
      traversal_stack.push(std::make_pair(false, node_ptr));
    }

    while (!traversal_stack.empty())
    {
      auto top_node = traversal_stack.top();
      traversal_stack.pop();
      if (top_node.first)
      {
        sorted_nodes.push_back(top_node.second);
        continue;
      }
      if (visited_nodes.find(top_node.second) != visited_nodes.end())
        continue;

      visited_nodes.insert(top_node.second);
      traversal_stack.push(std::make_pair(true, top_node.second));
      if (top_node.second->is_operation_node())
      {
        auto &operands = top_node.second->get_operands();

        for (auto &operand_ptr : operands)
        {
          if (visited_nodes.find(operand_ptr) == visited_nodes.end())
          {
            traversal_stack.push(std::make_pair(false, operand_ptr));
          }
        }
      }
    }
  }
  // remove dead parents
  for (auto &node_ptr : sorted_nodes)
  {
    node_ptr->clear_parents();
    if (node_ptr->is_operation_node())
    {
      for (auto &operand : node_ptr->get_operands())
      {
        operand->insert_parent_label(node_ptr->get_label());
      }
    }
  }
}

bool DAG::update_if_output_entry(const std::string &output_label, const Ptr &node)
{
  auto it = outputs_nodes.find(output_label);

  if (it != outputs_nodes.end())
  {
    outputs_nodes[output_label] = node;
    return true;
  }

  return false;
}

bool DAG::is_output_node(const std::string &label)
{
  auto it = outputs_nodes.find(label);
  return it != outputs_nodes.end();
}
} // namespace ir
