#pragma once

#include "fhecompiler_const.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>
#include <vector>

extern ir::Program *program;

namespace datatype
{

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

  auto lhs_node_ptr = program->insert_node_in_dataflow<T1>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<T2>(rhs);

  std::string old_label = lhs.get_label();

  lhs.set_new_label();
  auto new_operation_node_ptr =
      program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), term_type);

  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    if (table_entry.get_entry_type() == ir::ConstantTableEntryType::output)
      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
  }
}

template <typename T1, typename T2, typename T3>
T1 operate_binary(const T2 &lhs, const T3 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto lhs_node_ptr = program->insert_node_in_dataflow<T2>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<T3>(rhs);
  return operate<T1>(opcode, {lhs_node_ptr, rhs_node_ptr}, term_type);
}

template <typename T> T operate_unary(const T &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = program->insert_node_in_dataflow<T>(rhs);
  return operate<T>(opcode, {rhs_node_ptr}, term_type);
}

template <typename T> T &operate_assignement(T &lhs, const T &rhs, ir::TermType term_type)
{
  auto lhs_entry = program->get_entry_form_constants_table(lhs.get_label());

  if (lhs_entry == std::nullopt)
    throw(" object node doesnt exist in dataflow when trying to overload assign operator for Ciphertext ");

  if (program->type_of(lhs.get_label()) == ir::ConstantTableEntryType::output)
  {
    auto ct_copy_node_ptr = program->insert_node_in_dataflow<T>(rhs);
    // inserting new output in data flow as assignement, and in the constatns_table but this time we insert it as a
    // symbol with tag
    std::string old_label = lhs.get_label();
    lhs.set_new_label();
    program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
    auto new_assign_operation =
        program->insert_operation_node_in_dataflow(ir::OpCode::assign, {ct_copy_node_ptr}, lhs.get_label(), term_type);
  }

  else
    lhs.set_label(rhs.get_label());

  return lhs;
}

inline void operate_in_constants_table(const std::string &label, const std::string &tag, fhecompiler::VarType var_type)
{
  if (tag.length())
  {

    ir::ConstantTableEntryType entry_type;
    if (var_type == fhecompiler::VarType::input)
      entry_type = ir::ConstantTableEntryType::input;
    else if (var_type == fhecompiler::VarType::output)
      entry_type = ir::ConstantTableEntryType::output;
    else if (var_type == fhecompiler::VarType::constant)
      entry_type = ir::ConstantTableEntryType::constant;
    else
      entry_type = ir::ConstantTableEntryType::constant;

    ir::ConstantTableEntry::EntryValue entry_value = tag;
    program->insert_entry_in_constants_table({label, {entry_type, entry_value}});
  }
}

template <typename T> /* T cannot be a Ciphertext since we don't support Ciphertext evaluation during compile time*/
bool is_compile_time_evaluation_possible(const T &lhs, const T &rhs)
{
  ir::ConstantTableEntryType lhs_entry_type = program->type_of(lhs.get_label());
  return (lhs_entry_type == ir::ConstantTableEntryType::constant &&
          lhs_entry_type == program->type_of(rhs.get_label()));
}

} // namespace datatype
