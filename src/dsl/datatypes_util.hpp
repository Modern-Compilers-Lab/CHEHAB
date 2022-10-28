#pragma once

#include "fhecompiler_const.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>
#include <vector>

extern ir::Program *program;

namespace datatype
{

inline bool is_tracked_object(const std::string &label);

using Ptr = std::shared_ptr<ir::Term>;

template <typename T>
T operate(ir::OpCode opcode, const std::vector<Ptr> &operands, ir::TermType term_type, bool is_output = false)
{
  T new_T("");
  program->insert_operation_node_in_dataflow(opcode, operands, new_T.get_label(), term_type);
  return new_T;
}

template <typename T1, typename T2>
void compound_operate(T1 &lhs, const T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{

  auto lhs_node_ptr = program->find_node_in_dataflow(lhs.get_label());
  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());

  if (rhs_node_ptr == nullptr || lhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  std::string old_label = lhs.get_label();

  lhs.set_new_label();

  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    bool is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    if (is_output || is_tracked_object(old_label))
    {
      if (is_output)
        program->delete_node_from_outputs(old_label);

      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
    }
  }
  auto new_operation_node_ptr =
    program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), term_type);
}

template <typename T1, typename T2, typename T3>
T1 operate_binary(const T2 &lhs, const T3 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto lhs_node_ptr = program->find_node_in_dataflow(lhs.get_label());
  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());
  if (lhs_node_ptr == nullptr || rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  return operate<T1>(opcode, {lhs_node_ptr, rhs_node_ptr}, term_type);
}

template <typename T1, typename T2>
T1 operate_unary(const T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());
  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  return operate<T1>(opcode, {rhs_node_ptr}, term_type);
}

template <typename T>
T &operate_assignement(T &lhs, const T &rhs, ir::TermType term_type)
{
  auto lhs_entry = program->get_entry_form_constants_table(lhs.get_label());

  if (lhs_entry == std::nullopt)
    throw(" object node doesnt exist in dataflow when trying to overload assign operator for Ciphertext ");

  auto copy_node_ptr = program->find_node_in_dataflow(rhs.get_label());

  if (copy_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  if ((program->type_of(lhs.get_label()) == ir::ConstantTableEntryType::output))
  {
    // inserting new output in data flow as assignement, and in the constatns_table but this time we insert it as a
    // symbol with tag
    std::string old_label = lhs.get_label();
    lhs.set_new_label();
    program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
    auto new_assign_operation =
      program->insert_operation_node_in_dataflow(ir::OpCode::assign, {copy_node_ptr}, lhs.get_label(), term_type);
  }
  else
  {
    if (is_tracked_object(lhs.get_label()))
    {
      auto new_assign_operation =
        program->insert_operation_node_in_dataflow(ir::OpCode::assign, {copy_node_ptr}, lhs.get_label(), term_type);
    }

    else
      lhs.set_label(rhs.get_label());
  }

  return lhs;
}

inline void operate_in_constants_table(const std::string &label, const std::string &tag, fhecompiler::VarType var_type)
{

  if (tag.length() == 0 && var_type == fhecompiler::VarType::temp)
    return;

  else
  {

    std::string tag_to_insert = (tag.length() ? tag : label);

    ir::ConstantTableEntryType entry_type;
    if (var_type == fhecompiler::VarType::input)
      entry_type = ir::ConstantTableEntryType::input;
    else if (var_type == fhecompiler::VarType::output)
      entry_type = ir::ConstantTableEntryType::output;
    else if (var_type == fhecompiler::VarType::constant)
      entry_type = ir::ConstantTableEntryType::constant;
    else
      entry_type = ir::ConstantTableEntryType::temp;

    ir::ConstantTableEntry::EntryValue entry_value = tag_to_insert;
    program->insert_entry_in_constants_table({label, {entry_type, entry_value}});
  }
}

template <typename T> /* T cannot be a Ciphertext since we don't support Ciphertext evaluation during compile time*/
bool is_compile_time_evaluation_possible(const T &lhs, const T &rhs)
{
  ir::ConstantTableEntryType lhs_entry_type = program->type_of(lhs.get_label());
  return lhs_entry_type == ir::ConstantTableEntryType::constant && lhs_entry_type == program->type_of(rhs.get_label());
}

inline bool is_tracked_object(const std::string &label)
{
  auto table_entry = program->get_entry_form_constants_table(label);

  if (table_entry == std::nullopt)
    return false;

  ir::ConstantTableEntry lhs_table_entry_value = *table_entry;

  ir::ConstantTableEntry::EntryValue entry_value = lhs_table_entry_value.get_entry_value();

  return entry_value.get_tag().length() > 0;
}

template <typename T>
void operate_copy(const T &lhs, const T &t_copy, ir::TermType term_type)
{
  auto copy_node_ptr = program->insert_node_in_dataflow<T>(t_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {copy_node_ptr}, lhs.get_label(), term_type);
}

template <typename T>
void operate_move(T &lhs, T &&t_move, ir::TermType term_type)
{
  if (is_tracked_object(lhs.get_label()))
  {
    auto move_node_ptr = program->insert_node_in_dataflow<T>(t_move);
    program->insert_operation_node_in_dataflow(ir::OpCode::assign, {move_node_ptr}, lhs.get_label(), term_type);
  }
  else
  {
    lhs.set_label(t_move.get_label());
  }
}

template <typename T>
void compound_operate_with_raw(T &lhs, datatype::rawData raw_data, ir::OpCode opcode, ir::TermType term_type)
{

  Ptr rhs_term = std::make_shared<ir::Term>(raw_data, ir::TermType::rawDataType);
  Ptr lhs_term = program->find_node_in_dataflow(lhs.get_label());

  if (lhs_term == nullptr)
    throw(" operand not defined, maybe it is a temporary and it is only declared \n");

  std::string old_label = lhs.get_label();
  lhs.set_new_label();
  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    bool is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    if (is_output || is_tracked_object(old_label))
    {
      if (is_output)
        program->delete_node_from_outputs(old_label);

      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
    }
  }
  auto new_operation_node_ptr =
    program->insert_operation_node_in_dataflow(opcode, {lhs_term, rhs_term}, lhs.get_label(), term_type);
}

template <typename T>
T operate_with_raw(const T &lhs, datatype::rawData raw_data, ir::OpCode opcode, ir::TermType term_type)
{

  T new_T("");
  Ptr rhs_term = std::make_shared<ir::Term>(raw_data, ir::TermType::rawDataType);
  Ptr lhs_term = program->find_node_in_dataflow(lhs.get_label());
  if (lhs_term == nullptr)
    throw(" operand not defined, maybe it is a temporary and it is only declared \n");

  program->insert_operation_node_in_dataflow(opcode, {lhs_term, rhs_term}, new_T.get_label(), term_type);

  return new_T;
}

} // namespace datatype
