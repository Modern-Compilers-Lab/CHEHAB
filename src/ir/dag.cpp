#include "dag.hpp"
#include <algorithm>
#include <optional>
#include <stack>
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
  node_ptr_from_label.erase(it);
}

void DAG::delete_node_from_outputs(const std::string &key)
{
  auto it = this->outputs_nodes.find(key);
  if (it != this->outputs_nodes.end())
  {
    this->outputs_nodes.erase(it);
  }
}

void DAG::apply_topological_sort(bool clear_existing_order)
{

  if (outputs_nodes_topsorted.size() && clear_existing_order == false)
    return;

  outputs_nodes_topsorted.clear();

  std::stack<std::pair<bool, Ptr>> traversal_stack;

  std::unordered_set<std::string> visited_labels;

  for (auto &e : outputs_nodes)
  {

    auto &node_ptr = e.second;

    if (visited_labels.find(node_ptr->get_label()) == visited_labels.end())
    {
      traversal_stack.push(std::make_pair(false, node_ptr));
    }

    while (!traversal_stack.empty())
    {
      auto top_node = traversal_stack.top();
      traversal_stack.pop();
      if (top_node.first)
      {
        outputs_nodes_topsorted.push_back(top_node.second);
        continue;
      }
      if (visited_labels.find(top_node.second->get_label()) != visited_labels.end())
        continue;

      visited_labels.insert(top_node.second->get_label());
      traversal_stack.push(std::make_pair(true, top_node.second));
      if (top_node.second->is_operation_node())
      {
        // const std::vector<Ptr> &operands = *(top_node.second)->get_operands();
        auto &operands = top_node.second->get_operands();

        for (auto &operand_ptr : operands)
        {
          if (visited_labels.find(operand_ptr->get_label()) == visited_labels.end())
          {
            traversal_stack.push(std::make_pair(false, operand_ptr));
          }
        }
      }
    }
  }
}

} // namespace ir
