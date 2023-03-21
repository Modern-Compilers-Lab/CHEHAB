#include "cse_pass.hpp"
#include "ir_utils.hpp"
#include <sstream>

namespace fheco_passes
{

/*
std::string CSE::calculate_id(const ir::Term::Ptr &term)
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
*/

bool CSE::check_inputs_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  return lhs == rhs;
}

bool CSE::check_raw_data_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  return lhs->get_label() == rhs->get_label();
}

bool CSE::check_scalars_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  if (lhs->get_term_type() != ir::TermType::scalar || rhs->get_term_type() != ir::TermType::scalar)
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

bool CSE::check_plains_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  if (lhs->get_term_type() != ir::TermType::plaintext || rhs->get_term_type() != ir::TermType::plaintext)
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

bool CSE::check_constants_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  if (program->type_of(lhs->get_label()) != ir::ConstantTableEntryType::constant)
    throw("lhs must be a constant in check_constants_equality");

  if (program->type_of(rhs->get_label()) != ir::ConstantTableEntryType::constant)
    throw("rhs must be a constant in check_constants_equality");

  if (lhs->get_term_type() != rhs->get_term_type())
    throw("operands must have same term type in check_constants_equality");

  ir::TermType type = lhs->get_term_type(); // same as rhs->get_term_type()

  if (type == ir::TermType::scalar)
  {
    return check_scalars_equality(lhs, rhs);
  }
  else if (type == ir::TermType::plaintext)
  {
    return check_plains_equality(lhs, rhs);
  }

  return false;
}

bool CSE::check_syntactical_equality(const ir::Term::Ptr &lhs, const ir::Term::Ptr &rhs)
{
  // this function is implemented in which we assume that it is called during a bottom-up pass for performance reasons

  if (lhs->get_opcode() != rhs->get_opcode())
    return false;

  // at this level lhs and rhs have the same opcode

  if (lhs->get_term_type() != rhs->get_term_type())
    return false;

  if (lhs->get_term_type() == ir::TermType::rawData)
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

  /*
    This part is not reached when calling cse_apply2
  */
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

/*

//Native implementation that we keep in case we need it for debugging

void CSE::apply_cse()
{

  std::unordered_set<ir::Term::Ptr> processed_nodes;

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
*/

void CSE::apply_cse2(bool allow_assign_insertion)
{

  // Please note that this pass may insert assign operation nodes

  std::unordered_set<ir::Term::Ptr> processed_constants;
  std::unordered_map<SEid, ir::Term::Ptr, SEidHash> calculated_expressions_ids;

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
      SEid expression_id = node;
      auto it = calculated_expressions_ids.find(expression_id);
      if (it != calculated_expressions_ids.end())
      {
        // all parents of node needs to point to processed_node instead of node
        if (parents_labels.size())
        {
          for (auto &parent_label : parents_labels)
          {
            auto parent_node = program->find_node_in_dataflow(parent_label);
            if (parent_node == nullptr)
              continue;

            parent_node->replace_operand_with(node, it->second);
          }
        }
        else
        {
          /*
            if the node doesn't have parents then in that case we convert the operation node to an assignement node. in
            best case will gain at most one operation each time we face this scenarion
          */
          if (allow_assign_insertion)
          {
            node->set_opcode(ir::OpCode::assign);
            node->clear_operands();
            node->set_operands({it->second});
          }
        }
      }
      else
        calculated_expressions_ids[expression_id] = node;
    }
  }
}

} // namespace fheco_passes
