#include "cse_pass.hpp"
#include "ir_utils.hpp"
#include <sstream>

namespace fheco_passes
{

std::string CSE::calculate_id(const ir::Program::Ptr &term)
{
  std::stringstream ss;
  if (term->is_operation_node() == false)
    ss << term;

  else if (term->get_operands().size() == 1)
  {
    ss << "(" << ir::str_opcode[term->get_opcode()] << ")" << term->get_operands()[0];
  }
  else if (term->get_operands().size() == 2)
  {
    ss << term->get_operands()[0] << "(" << ir::str_opcode[term->get_opcode()] << term->get_operands()[1];
  }
  return ss.str();
}

bool CSE::check_inputs_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  return lhs == rhs;
}

bool CSE::check_raw_data_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  return lhs->get_label() == rhs->get_label();
}

bool CSE::check_scalars_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  if (lhs->get_term_type() != ir::scalarType || rhs->get_term_type() != ir::scalarType)
    throw("both lhs and rhs must be scalars in check_scalars_equality");

  auto lhs_const_table_entry = program->get_entry_form_constants_table(lhs->get_label());

  if (lhs_const_table_entry == std::nullopt)
    throw("lhs is expected to have an entry in constants table");

  auto rhs_const_table_entry = program->get_entry_form_constants_table(rhs->get_label());

  if (rhs_const_table_entry == std::nullopt)
    throw("rhs is expected to have an entry in constants table");

  ir::ConstantValue lhs_const_value = *(*lhs_const_table_entry).get().get_entry_value().value;
  ir::ConstantValue rhs_const_value = *(*rhs_const_table_entry).get().get_entry_value().value;

  return ir::get_constant_value_as_double(lhs_const_value) == ir::get_constant_value_as_double(rhs_const_value);
}

bool CSE::check_plains_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  if (lhs->get_term_type() != ir::plaintextType || rhs->get_term_type() != ir::plaintextType)
    throw("both lhs and rhs must be scalars in check_scalars_equality");

  if (program->type_of(lhs->get_label()) != ir::ConstantTableEntryType::constant)
    return false;

  if (program->type_of(rhs->get_label()) != ir::ConstantTableEntryType::constant)
    return false;

  auto lhs_const_table_entry = program->get_entry_form_constants_table(lhs->get_label());

  if (lhs_const_table_entry == std::nullopt)
    throw("lhs is expected to have an entry in constants table");

  auto rhs_const_table_entry = program->get_entry_form_constants_table(rhs->get_label());

  if (rhs_const_table_entry == std::nullopt)
    throw("rhs is expected to have an entry in constants table");

  ir::ConstantValue lhs_const_value = *(*lhs_const_table_entry).get().get_entry_value().value;
  ir::ConstantValue rhs_const_value = *(*rhs_const_table_entry).get().get_entry_value().value;

  // checks vectors equality
  std::vector<double> lhs_vector;
  ir::get_constant_value_as_vector_of_double(lhs_const_value, lhs_vector);
  std::vector<double> rhs_vector;
  ir::get_constant_value_as_vector_of_double(rhs_const_value, rhs_vector);

  if (lhs_vector.size() != rhs_vector.size())
    throw("const plains vectors must have the same size");

  for (size_t i = 0; i < lhs_vector.size(); i++)
    if (lhs_vector[i] != rhs_vector[i])
      return false;

  return true;
}

bool CSE::check_constants_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  if (program->type_of(lhs->get_label()) != ir::ConstantTableEntryType::constant)
    throw("lhs must be a constant in check_constants_equality");

  if (program->type_of(rhs->get_label()) != ir::ConstantTableEntryType::constant)
    throw("rhs must be a constant in check_constants_equality");

  if (lhs->get_term_type() != rhs->get_term_type())
    throw("operands must have same term type in check_constants_equality");

  ir::TermType type = lhs->get_term_type(); // same as rhs->get_term_type()

  if (type == ir::scalarType)
  {
    return check_scalars_equality(lhs, rhs);
  }
  else if (type == ir::plaintextType)
  {
    return check_plains_equality(lhs, rhs);
  }

  return false;
}

bool CSE::check_syntactical_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  // this function is implemented in which we assume that it is called during a bottom-up pass for performance reasons

  if (lhs->get_opcode() != rhs->get_opcode())
    return false;

  // at this level lhs and rhs have the same opcode

  if (lhs->get_term_type() != rhs->get_term_type())
    return false;

  if (lhs->get_term_type() == ir::rawDataType)
  {
    return check_raw_data_equality(lhs, rhs);
  }

  if (
    program->type_of(lhs->get_label()) == ir::ConstantTableEntryType::constant &&
    program->type_of(rhs->get_label()) == ir::ConstantTableEntryType::constant)
  {
    return check_constants_equality(lhs, rhs); // checking equality according to term type
  }

  if (
    program->type_of(lhs->get_label()) == ir::ConstantTableEntryType::input &&
    program->type_of(rhs->get_label()) == ir::ConstantTableEntryType::input)
  {
    return check_inputs_equality(lhs, rhs);
  }

  if (lhs->get_opcode() != ir::OpCode::undefined && rhs->get_opcode() != ir::OpCode::undefined)
  {
    if (lhs->get_operands().size() != rhs->get_operands().size())
      return false;

    for (size_t i = 0; i < lhs->get_operands().size(); i++)
    {
      if (lhs->get_operands()[i] != rhs->get_operands()[i])
        return false;
    }
    return true;
  }
  return lhs == rhs;
}

void CSE::apply_cse()
{

  std::unordered_set<ir::Program::Ptr> processed_nodes;

  auto nodes = program->get_dataflow_sorted_nodes(true);

  for (auto &node : nodes)
  {
    auto parents_labels = node->get_parents_labels();
    for (auto &parent_label : parents_labels)
    {
      if (program->find_node_in_dataflow(parent_label) == nullptr)
      {
        std::cout << "this is a problem!\n";
      }
    }
  }

  std::cout << nodes.size() << "\n";
  for (size_t i = 0; i < nodes.size(); i++)
  {
    auto node = nodes[i];
    auto parents_labels = node->get_parents_labels();
    bool cse_applied = false;
    for (auto &processed_node : processed_nodes)
    {
      if (check_syntactical_equality(processed_node, node))
      {
        // all parents of node needs to point to processed_node instead of node
        for (auto &parent_label : parents_labels)
        {
          auto parent_node = program->find_node_in_dataflow(parent_label);
          // if (parent_node == nullptr)

          parent_node->replace_operand_with(node, processed_node);
        }
        cse_applied = true;
        break;
      }
    }
    if (!cse_applied)
      processed_nodes.insert(node);
  }
}

void CSE::apply_cse2()
{

  std::unordered_set<ir::Program::Ptr> processed_constants;
  std::unordered_map<std::string, ir::Program::Ptr> calculated_ids;

  auto nodes = program->get_dataflow_sorted_nodes(true);
  for (size_t i = 0; i < nodes.size(); i++)
  {
    auto node = nodes[i];
    auto parents_labels = node->get_parents_labels();
    bool cse_applied = false;
    if (node->is_operation_node() == false && program->type_of(node->get_label()) != ir::ConstantTableEntryType::input)
    {
      for (auto &processed_constant : processed_constants)
      {
        if (check_syntactical_equality(processed_constant, node))
        {
          // all parents of node needs to point to processed_node instead of node
          for (auto &parent_label : parents_labels)
          {
            auto parent_node = program->find_node_in_dataflow(parent_label);
            if (parent_node == nullptr)
              continue;

            parent_node->replace_operand_with(node, processed_constant);
          }
          cse_applied = true;
          break;
        }
      }
      if (!cse_applied)
        processed_constants.insert(node);
    }
    else
    {
      std::string node_id = calculate_id(node);
      auto it = calculated_ids.find(node_id);
      if (it != calculated_ids.end())
      {
        // all parents of node needs to point to processed_node instead of node
        for (auto &parent_label : parents_labels)
        {
          auto parent_node = program->find_node_in_dataflow(parent_label);
          if (parent_node == nullptr)
            continue;

          parent_node->replace_operand_with(node, it->second);
        }
        cse_applied = true;
      }
      if (!cse_applied)
        calculated_ids[node_id] = node;
    }
  }
}

} // namespace fheco_passes
