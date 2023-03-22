#pragma once

#include "compiler.hpp"
#include "fhecompiler_const.hpp"
#include "term.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

namespace fhecompiler
{

template <typename T>
T operate(ir::OpCode opcode, const std::vector<ir::Term::Ptr> &operands, ir::TermType term_type, bool is_output = false)
{
  T new_T("");
  Compiler::get_active()->insert_operation_node_in_dataflow(opcode, operands, new_T.get_label(), term_type);

  if (operands.size() == 2)
    Compiler::operate_binary(opcode, operands[0]->get_label(), operands[1]->get_label(), new_T.get_label(), is_output);
  else if (operands.size() == 1)
    Compiler::operate_unary(opcode, operands[0]->get_label(), new_T.get_label(), is_output);
  else
    throw std::logic_error("invalide number of operands in operate");

  return new_T;
}

template <typename T1, typename T2>
void compound_operate(T1 &lhs, const T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{

  auto lhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(lhs.get_label());
  auto rhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(rhs.get_label());

  if (rhs_node_ptr == nullptr || lhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  std::string old_label = lhs.get_label();

  lhs.set_new_label();

  bool is_output = false;
  auto table_entry_opt = Compiler::get_active()->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {

    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    bool is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;

    if (is_output || Compiler::get_active()->is_tracked_object(old_label))
    {
      // Compiler::get_active()->delete_node_from_dataflow(old_label);
      if (!is_input)
      {
        Compiler::get_active()->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
      }

      if (is_output)
      {
        Compiler::get_active()->delete_node_from_outputs(old_label);
      }
    }
  }
  auto new_operation_node_ptr = Compiler::get_active()->insert_operation_node_in_dataflow(
    opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), term_type);
  new_operation_node_ptr->set_inplace();

  Compiler::operate_binary(opcode, old_label, rhs.get_label(), lhs.get_label(), is_output);
}

template <typename T1, typename T2, typename T3>
T1 operate_binary(const T2 &lhs, const T3 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto lhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(lhs.get_label());
  auto rhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(rhs.get_label());
  if (lhs_node_ptr == nullptr || rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  return operate<T1>(opcode, std::vector<std::shared_ptr<ir::Term>>({lhs_node_ptr, rhs_node_ptr}), term_type);
}

template <typename T1, typename T2>
T1 operate_unary(const T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(rhs.get_label());
  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  return operate<T1>(opcode, std::vector<std::shared_ptr<ir::Term>>({rhs_node_ptr}), term_type);
}

template <typename T1, typename T2>
void compound_operate_unary(T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(rhs.get_label());
  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  std::string old_label = rhs.get_label();

  rhs.set_new_label();
  bool is_output = false;

  auto table_entry_opt = Compiler::get_active()->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    bool is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;
    if (is_output || Compiler::get_active()->is_tracked_object(old_label))
    {
      // Compiler::get_active()->delete_node_from_dataflow(old_label);
      if (!is_input)
      {
        Compiler::get_active()->insert_new_entry_from_existing_with_delete(rhs.get_label(), old_label);
      }

      if (is_output)
      {
        Compiler::get_active()->delete_node_from_outputs(old_label);
      }
    }
  }
  auto new_operation_node_ptr = Compiler::get_active()->insert_operation_node_in_dataflow(
    opcode, std::vector<ir::Term::Ptr>({rhs_node_ptr}), rhs.get_label(), term_type);
  new_operation_node_ptr->set_inplace();

  if (opcode != ir::OpCode::square)
    throw std::logic_error("operation other than suqare in compound_operate_unary");

  Compiler::operate_unary(ir::OpCode::square, old_label, rhs.get_label(), is_output);
}

template <typename T>
T &operate_assignement(T &lhs, const T &rhs, ir::TermType term_type)
{

  if (lhs.get_label() == rhs.get_label())
    return lhs;

  auto lhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(lhs.get_label());

  auto rhs_node_ptr = Compiler::get_active()->find_node_in_dataflow(rhs.get_label());

  bool is_output = Compiler::get_active()->type_of(lhs.get_label()) == ir::ConstantTableEntryType::output;

  bool is_input = Compiler::get_active()->type_of(lhs.get_label()) == ir::ConstantTableEntryType::input;

  // if (is_tracked_object(lhs.get_label()))
  //{
  //  inserting new output in data flow as assignement, and in the constatns_table but this time we
  //  insert it as a symbol with tag

  /*
  if (lhs_node_ptr == nullptr)
    throw("object node doesnt exist in dataflow when trying to overload assign operator");
  */

  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  if (Compiler::get_active()->is_tracked_object(lhs.get_label()))
  {

    std::string old_label = lhs.get_label();
    lhs.set_new_label();

    if (!is_input)
    {
      Compiler::get_active()->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
    }

    if (is_output)
      Compiler::get_active()->delete_node_from_outputs(old_label);

    // Compiler::get_active()->delete_node_from_dataflow(old_label);
    auto new_assign_operation = Compiler::get_active()->insert_operation_node_in_dataflow(
      ir::OpCode::assign, {rhs_node_ptr}, lhs.get_label(), term_type);

    Compiler::operate_unary(ir::OpCode::assign, rhs.get_label(), lhs.get_label(), is_output);
  }
  else
  {
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
    Compiler::get_active()->insert_entry_in_constants_table({label, {entry_type, entry_value}});
  }
}

template <typename T> /* T cannot be a Ciphertext since we don't support Ciphertext evaluation during compile time*/
bool is_compile_time_evaluation_possible(const T &lhs, const T &rhs)
{
  ir::ConstantTableEntryType lhs_entry_type = Compiler::get_active()->type_of(lhs.get_label());
  return lhs_entry_type == ir::ConstantTableEntryType::constant &&
         lhs_entry_type == Compiler::get_active()->type_of(rhs.get_label());
}

template <typename T>
void operate_copy(const T &lhs, const T &t_copy, ir::TermType term_type)
{
  auto copy_node_ptr = Compiler::get_active()->insert_node_in_dataflow<T>(t_copy);
  Compiler::get_active()->insert_operation_node_in_dataflow(
    ir::OpCode::assign, {copy_node_ptr}, lhs.get_label(), term_type);

  Compiler::operate_unary(
    ir::OpCode::assign, t_copy.get_label(), lhs.get_label(),
    Compiler::get_active()->type_of(lhs.get_label()) == ir::ConstantTableEntryType::output);
}

template <typename T>
void operate_move(T &lhs, T &&t_move, ir::TermType term_type)
{
  // if (is_tracked_object(lhs.get_label()))
  //{
  auto move_node_ptr = Compiler::get_active()->insert_node_in_dataflow<T>(t_move);
  Compiler::get_active()->insert_operation_node_in_dataflow(
    ir::OpCode::assign, {move_node_ptr}, lhs.get_label(), term_type);

  Compiler::operate_unary(
    ir::OpCode::assign, t_move.get_label(), lhs.get_label(),
    Compiler::get_active()->type_of(lhs.get_label()) == ir::ConstantTableEntryType::output);
  //}
  // else
  //{
  //  lhs.set_label(t_move.get_label());
  //}
}

template <typename T>
void compound_operate_with_raw(T &lhs, datatype::rawData raw_data, ir::OpCode opcode, ir::TermType term_type)
{

  ir::Term::Ptr rhs_term = std::make_shared<ir::Term>(raw_data, ir::TermType::rawData);
  ir::Term::Ptr lhs_term = Compiler::get_active()->find_node_in_dataflow(lhs.get_label());

  if (lhs_term == nullptr)
    throw(" operand not defined, maybe it is a temporary and it is only declared \n");

  std::string old_label = lhs.get_label();
  lhs.set_new_label();
  auto table_entry_opt = Compiler::get_active()->get_entry_form_constants_table(old_label);
  bool is_output = false;
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    bool is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;
    if (is_output || Compiler::get_active()->is_tracked_object(old_label))
    {
      if (is_output)
        Compiler::get_active()->delete_node_from_outputs(old_label);

      if (!is_input)
      {
        Compiler::get_active()->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
      }
    }
  }
  auto new_operation_node_ptr =
    Compiler::get_active()->insert_operation_node_in_dataflow(opcode, {lhs_term, rhs_term}, lhs.get_label(), term_type);
  new_operation_node_ptr->set_inplace();

  if (opcode != ir::OpCode::rotate)
    throw std::logic_error("operation other than rotate in compound_operate_with_raw");

  Compiler::operate_rotate(old_label, lhs.get_label(), std::stoi(raw_data), lhs.get_label(), is_output);
}

template <typename T>
T operate_with_raw(const T &lhs, datatype::rawData raw_data, ir::OpCode opcode, ir::TermType term_type)
{

  T new_T("");

  ir::Term::Ptr rhs_term = std::make_shared<ir::Term>(raw_data, ir::TermType::rawData);
  ir::Term::Ptr lhs_term = Compiler::get_active()->find_node_in_dataflow(lhs.get_label());
  if (lhs_term == nullptr)
    throw(" operand not defined, maybe it is a temporary and it is only declared \n");

  Compiler::get_active()->insert_operation_node_in_dataflow(opcode, {lhs_term, rhs_term}, new_T.get_label(), term_type);

  if (opcode != ir::OpCode::rotate)
    throw std::logic_error("operation other than rotate in operate_with_raw");

  Compiler::operate_rotate(lhs.get_label(), std::stoi(raw_data), new_T.get_label());

  return new_T;
}

} // namespace fhecompiler
