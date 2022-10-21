#pragma once

#include "term.hpp"
#include <memory>
#include <deque>
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

  std::vector<Ptr> nodes_ptrs_topsorted; // nodes topologically sorted

  std::unordered_map<std::string, Ptr> node_ptr_from_label;

public:
  DAG() = default;

  ~DAG() {}

  void insert_node(Ptr node_ptr);

  void delete_node(Ptr node_ptr);

  void apply_topological_sort();

  Ptr find_node(std::string node_label) const;

  const std::vector<Ptr> &get_node_ptrs() const { return nodes_ptrs; }

  const std::vector<Ptr> &get_nodes_ptrs_topsorted() const { return nodes_ptrs_topsorted; }
};

} // namespace ir
