#include"dag.hpp"

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
  if(it != node_ptr_from_label.end()) return (*it).second; 
  return nullptr;
}

} // namespace ir

