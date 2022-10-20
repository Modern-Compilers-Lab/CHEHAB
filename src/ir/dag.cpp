#include "dag.hpp"
#include <algorithm>
#include <stack>
#include <unordered_set>

namespace ir
{

using Ptr = std::shared_ptr<Term>;

void DAG::insert_node(Ptr node)
{
  this->nodes_ptrs.push_back(node);
  this->node_ptr_from_label[node->get_label()] = node;
}

Ptr DAG::find_node(std::string node_label) const
{
  auto it = node_ptr_from_label.find(node_label);
  if (it != node_ptr_from_label.end())
    return (*it).second;
  return nullptr;
}

void DAG::apply_topological_sort()
{
  // iterative algorithm

  if (nodes_ptrs_topsorted.size())
    return;

  std::stack<Ptr> traversal_stack;

  std::unordered_set<std::string> visited_labels;

  for (auto &node_ptr : nodes_ptrs)
  {
    if (visited_labels.find(node_ptr->get_label()) != visited_labels.end())
      continue;

    visited_labels.insert(node_ptr->get_label());
    traversal_stack.push(node_ptr);

    while (!traversal_stack.empty())
    {
      auto &top_node_ptr = traversal_stack.top();
      traversal_stack.pop();
      nodes_ptrs_topsorted.push_back(top_node_ptr);
      if (top_node_ptr->get_operands() != std::nullopt)
      {
        const auto &operands_nodes_ptrs = *(top_node_ptr->get_operands());
        for (auto &operand_node_ptr : operands_nodes_ptrs)
        {
          if (visited_labels.find(operand_node_ptr->get_label()) == visited_labels.end())
          {
            visited_labels.insert(operand_node_ptr->get_label());
            traversal_stack.push(operand_node_ptr);
          }
        }
      }
    }
  }
}

} // namespace ir
