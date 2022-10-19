#pragma once

#include "term.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ir
{

class DAG
{

  using Ptr = std::shared_ptr<Term>;

private:
  std::vector<Ptr> nodes_ptrs;

  std::vector<Ptr> nodes_ptrs_topsorted; // node ntopologically sorted

  std::unordered_map<std::string, Ptr> node_ptr_from_label;

public:
  DAG() = default;

  ~DAG() {}

  void insert_node(Ptr node_ptr);

  void delete_node(Ptr node_ptr);

  void apply_topological_sort();

  void traverse_dag() const;

  Ptr find_node(std::string node_label) const;

  void traverse();
};

} // namespace ir
