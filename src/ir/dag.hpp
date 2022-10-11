#pragma once

#include<vector>
#include<memory>
#include<unordered_map>
#include"term.hpp"
#include<string>

namespace ir
{
  
class DAG
{

  using Ptr = std::shared_ptr<Term>;

  private:

  std::vector<Ptr> nodes_ptrs;

  std::unordered_map<std::string, Ptr> node_ptr_from_label;

  public:
    
  DAG() = default;

  void insert_node(Ptr node_ptr);

  void delete_node(Ptr node_ptr);

  void apply_topological_sort();

  void traverse_dag() const;

  Ptr find_node(std::string node_label) const;

};

}// namespace ir