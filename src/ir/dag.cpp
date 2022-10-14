#include"dag.hpp"
#include<stack>
#include<unordered_set>

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


void DAG::apply_topological_sort()
{
  //iterative algorithm

  if(nodes_ptrs_topsorted.size()) return;

  std::stack<Ptr> traversal_stack;

  std::unordered_set<std::string> visited_set;

  for(auto& node_ptr : this->nodes_ptrs)
  {

    std::string node_label = node_ptr->get_label();

    if(visited_set.find(node_label) != visited_set.end()) continue;

    visited_set.insert(node_label);
    
    traversal_stack.push(node_ptr);

    while(!traversal_stack.empty())
    {

      auto top_node_ptr = traversal_stack.top();
      traversal_stack.pop();
      
      if(top_node_ptr->get_operands() == std::nullopt)
      {
        nodes_ptrs_topsorted.push_back(top_node_ptr);
      }
      else
      {

        for(auto& operand_ptr : *(top_node_ptr->get_operands()))
        {
          traversal_stack.push(operand_ptr);
          visited_set.insert(operand_ptr->get_label());
        }
    
      }
    
    }

  }

}

std::unordered_map<ir::OpCode, std::string> opcode_map = 
{
  {ir::add,"+"},
  {ir::sub,"-"},
  {ir::mul,"*"},
  {ir::assign,"="}
};

std::string dfs(Ptr term, std::unordered_map<std::string, std::string>& expr)
{
  if(expr.find(term->get_label()) != expr.end()) return expr[term->get_label()];
  if(term->get_operands() == std::nullopt)
  {
    return expr[term->get_label()] = term->get_label();
  }
  else
  {
    const std::vector<Ptr>& operands = * term->get_operands();
    return expr[term->get_label()]= dfs(operands[0], expr)+opcode_map[term->get_opcode()]+dfs(operands[1], expr);
  }
}

void DAG::traverse()
{
  std::unordered_map<std::string, std::string> expr;
  this->apply_topological_sort();
  for(auto& node_ptr : this->nodes_ptrs)
  {
    std::cout << node_ptr->get_label() << " = " << dfs(node_ptr, expr) << "\n";
  }
}

} // namespace ir

