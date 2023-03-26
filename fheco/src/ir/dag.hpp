#pragma once

#include "term.hpp"
#include <map>
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
  std::map<std::string, Ptr> outputs_nodes;

  std::vector<Ptr> outputs_nodes_topsorted; // nodes topologically sorted

  std::unordered_map<std::string, Ptr> node_ptr_from_label;

public:
  DAG() = default;

  ~DAG() {}

  void insert_node(Ptr node_ptr, bool is_output = false);

  void delete_node(const std::string &node_label);

  void apply_topological_sort(bool clear_existing_order);

  Ptr find_node(const std::string &node_label) const;

  void delete_node_from_outputs(const std::string &key);

  const std::map<std::string, Ptr> &get_outputs_nodes() const { return outputs_nodes; }

  const std::vector<Ptr> &get_outputs_nodes_topsorted() const { return outputs_nodes_topsorted; }
};

} // namespace ir
