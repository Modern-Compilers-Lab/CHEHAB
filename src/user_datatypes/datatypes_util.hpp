#pragma once

#include"term.hpp"
#include"program.hpp"
#include<vector>
#include<memory>

extern ir::Program* program;

namespace datatype
{

using Ptr = std::shared_ptr<ir::Term>;

template< typename T1 >
T1 operate(ir::OpCode opcode, const std::vector<Ptr>& operands, ir::TermType term_type, bool is_output=false)
{
  T1 new_T("");
  program->insert_operation_node_in_dataflow(opcode, operands, new_T.get_label(), term_type);
  return new_T;
}


template< typename T2 >
void compound_operate(T2& lhs, const T2& rhs, ir::OpCode opcode, ir::TermType term_type)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<T2>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<T2>(rhs);

  std::string old_label = lhs.get_label();

  set_new_label(lhs);
  auto new_operation_node_ptr = program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), term_type);
  if(lhs_node_ptr->get_output_flag()) 
  {
    lhs_node_ptr->set_output_flag(false);
    new_operation_node_ptr->set_output_flag(true);
  }

  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if(table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry& table_entry = *table_entry_opt;
    if(table_entry.get_entry_type() == ir::ConstantTableEntry::ConstantTableEntryType::output)
      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
  }

}

template< typename T3 >
T3 operate_binary(const T3& lhs, const T3& rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto lhs_node_ptr = program->insert_node_in_dataflow<T3>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<T3>(rhs);
  return operate<T3>(opcode, {lhs_node_ptr, rhs_node_ptr}, term_type);
}

template< typename T4 >
T4 operate_unary(const T4& rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = program->insert_node_in_dataflow<T4>(rhs);
  return operate<T4>(opcode, {rhs_node_ptr}, term_type);
}

} // namespace datatypes

