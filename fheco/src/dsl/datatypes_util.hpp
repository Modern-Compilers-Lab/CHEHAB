#pragma once

#include "fhecompiler_const.hpp"
#include "ir_utils.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>
#include <vector>

extern ir::Program *program;

namespace fhecompiler
{

using Ptr = std::shared_ptr<ir::Term>;

template <typename T>
T operate(ir::OpCode opcode, const std::vector<Ptr> &operands, ir::TermType term_type, bool is_output = false)
{
  T new_T("");
  program->insert_operation_node_in_dataflow(opcode, operands, new_T.get_label(), term_type);
  return new_T;
}

template <typename T1, typename T2> /* T1 nor T2 be a Ciphertext since we don't support Ciphertext evaluation during
                                     * compile time
                                     */
bool is_compile_time_evaluation_possible(const T1 &lhs, const T2 &rhs)
{
  /*
    ir::ConstantTableEntryType lhs_entry_type = program->type_of(lhs.get_label());
    return lhs_entry_type == ir::ConstantTableEntryType::constant && lhs_entry_type ==
    program->type_of(rhs.get_label());
  */
  Ptr lhs_node_ptr = program->find_node_in_dataflow(lhs.get_label());
  Ptr rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());

  if (lhs_node_ptr->is_operation_node() == true)
    return false;
  else if (rhs_node_ptr->is_operation_node() == true)
    return false;

  auto lhs_const_value = program->get_entry_value_value(lhs.get_label());
  if (lhs_const_value == std::nullopt)
    return false;

  auto rhs_const_value = program->get_entry_value_value(rhs.get_label());
  if (rhs_const_value == std::nullopt)
    return false;

  return true;
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

  Ptr new_lhs_node;
  std ::string lhs_old_label = lhs.get_label();

  bool is_evaluation_possible = is_compile_time_evaluation_possible<T1, T2>(lhs, rhs);

  if (is_evaluation_possible)
  {
    ir::TermType lhs_term_type = lhs_node_ptr->get_term_type();
    ir::TermType rhs_term_type = rhs_node_ptr->get_term_type();

    if (lhs_term_type == ir::ciphertextType || rhs_term_type == ir::ciphertextType)
      throw("unexpected, ciphertexts evaluation is not available at compile time");

    if (lhs_term_type == ir::plaintextType && rhs_term_type == ir::plaintextType)
    {
      new_lhs_node = ir::fold_const_plain(lhs_node_ptr, rhs_node_ptr, opcode, program);
      lhs.set_label(new_lhs_node->get_label());
    }
    else if (lhs_term_type == ir::scalarType && rhs_term_type == ir::scalarType)
    {
      new_lhs_node = ir::fold_scalar(lhs_node_ptr, rhs_node_ptr, opcode, program);
      lhs.set_label(new_lhs_node->get_label());
    }
    else
    {
      /* this will be changed */
      new_lhs_node = ir::fold_plain_scalar(lhs_node_ptr, rhs_node_ptr, opcode, program);
      lhs.set_label(new_lhs_node->get_label());
    }
  }
  else
    lhs.set_new_label();

  auto table_entry_opt = program->get_entry_form_constants_table(lhs_old_label);
  bool is_output(false), is_input(false);

  if (table_entry_opt != std::nullopt)
  {

    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;
    is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;

    if (is_output || program->is_tracked_object(lhs_old_label))
    {
      // program->delete_node_from_dataflow(old_label);
      if (!is_input)
      {
        // we need to save constant_value in case that a compile time evaluation happend so that we make sure that we
        // have the right value
        if (is_evaluation_possible)
        {
          auto const_value_opt = program->get_entry_value_value(lhs.get_label());
          program->insert_new_entry_from_existing_with_delete(lhs.get_label(), lhs_old_label);
          if (const_value_opt != std::nullopt)
            program->set_constant_value_value(lhs.get_label(), *const_value_opt);
          else
            throw("constant value doesnt exist even though a compile time evaluation was performed");
        }
        else
          program->insert_new_entry_from_existing_with_delete(lhs.get_label(), lhs_old_label);
      }

      if (is_output)
      {
        program->delete_node_from_outputs(lhs_old_label);
      }
    }
  }

  if (is_evaluation_possible == false)
  {
    new_lhs_node =
      program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), term_type);
    new_lhs_node->set_inplace();
  }
  else
  {
    if (is_output)
      program->add_node_to_outputs_nodes(new_lhs_node);
  }
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

  auto is_evaluation_possible = is_compile_time_evaluation_possible<T2, T3>(lhs, rhs);

  if (is_evaluation_possible)
  {

    ir::TermType lhs_term_type = lhs_node_ptr->get_term_type();
    ir::TermType rhs_term_type = rhs_node_ptr->get_term_type();

    if (lhs_term_type == ir::ciphertextType || rhs_term_type == ir::ciphertextType)
      throw("ciphertexts evaluation is not available at compile time");

    if (lhs_term_type == ir::plaintextType && rhs_term_type == ir::plaintextType)
    {
      auto folded_term = ir::fold_const_plain(lhs_node_ptr, rhs_node_ptr, opcode, program);
      T1 new_T("");
      new_T.set_label(folded_term->get_label()); // this is the most important step
      return new_T;
    }

    else if (lhs_term_type == ir::scalarType && rhs_term_type == ir::scalarType)
    {
      auto folded_term = ir::fold_scalar(lhs_node_ptr, rhs_node_ptr, opcode, program);
      T1 new_T("");
      new_T.set_label(folded_term->get_label()); // this is the most important step
      return new_T;
    }
    else
    {
      auto folded_term = ir::fold_plain_scalar(lhs_node_ptr, rhs_node_ptr, opcode, program);
      T1 new_T("");
      new_T.set_label(folded_term->get_label()); // this is the most important step
      return new_T;
    }
  }
  return operate<T1>(opcode, std::vector<Ptr>({lhs_node_ptr, rhs_node_ptr}), term_type);
}

template <typename T1, typename T2>
T1 operate_unary(const T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());
  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  return operate<T1>(opcode, std::vector<Ptr>({rhs_node_ptr}), term_type);
}

template <typename T1, typename T2>
void compound_operate_unary(T2 &rhs, ir::OpCode opcode, ir::TermType term_type)
{
  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());
  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }
  std::string old_label = rhs.get_label();

  rhs.set_new_label();

  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if (table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &table_entry = *table_entry_opt;
    bool is_output = table_entry.get_entry_type() == ir::ConstantTableEntryType::output;
    bool is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;
    if (is_output || program->is_tracked_object(old_label))
    {
      // program->delete_node_from_dataflow(old_label);
      if (!is_input)
      {
        program->insert_new_entry_from_existing_with_delete(rhs.get_label(), old_label);
      }

      if (is_output)
      {
        program->delete_node_from_outputs(old_label);
      }
    }
  }
  auto new_operation_node_ptr =
    program->insert_operation_node_in_dataflow(opcode, std::vector<Ptr>({rhs_node_ptr}), rhs.get_label(), term_type);
  new_operation_node_ptr->set_inplace();
}

template <typename T>
void operate_copy(const T &lhs, const T &t_copy, ir::TermType term_type)
{
  auto copy_node_ptr = program->insert_node_in_dataflow<T>(t_copy);

  // auto copy_node_ptr = program->find_node_in_dataflow(t_copy.get_label());

  // node
  if (copy_node_ptr->is_operation_node() == false)
  {
    /* we care about const_value because it is the only thing that's important, all other information in constant table
     are related to the term itself, including type, label, tag ... */
    auto const_value = program->get_entry_value_value(t_copy.get_label());
    if (const_value != std::nullopt)
    {
      program->insert_or_update_entry_in_constants_table(lhs.get_label(), *const_value, false);
      program->insert_node_in_dataflow<T>(lhs);
    }
    else
    {
      // you have no information about t_copy, for example in case it is an input ciphertext
      program->insert_operation_node_in_dataflow(ir::OpCode::assign, {copy_node_ptr}, lhs.get_label(), term_type);
    }
  }
  else
  {
    program->insert_operation_node_in_dataflow(
      copy_node_ptr->get_opcode(), copy_node_ptr->get_operands(), lhs.get_label(), term_type);
  }
  // constants table entry
}

template <typename T>
void operate_move(T &lhs, T &&t_move, ir::TermType term_type)
{
  // if (is_tracked_object(lhs.get_label()))
  //{
  Ptr move_node_ptr = program->insert_node_in_dataflow<T>(t_move);

  if (move_node_ptr->is_operation_node() == false)
  {

    auto const_value = program->get_entry_value_value(move_node_ptr->get_label());
    if (const_value != std::nullopt)
    {
      program->insert_or_update_entry_in_constants_table(lhs.get_label(), *const_value, false);
      program->insert_node_in_dataflow<T>(lhs);
      program->reset_constant_value_value(move_node_ptr->get_label());
    }
    else
    {
      /* in that case the user means that he wants to perform a move that needs to be performed by the backend later;
      and we don't handle that for the moment, so basically we just generate a copy since it is more generic */
      program->insert_operation_node_in_dataflow(ir::OpCode::assign, {move_node_ptr}, lhs.get_label(), term_type);
      // throw("move semantics are not defined for inputs ciphertexts");
    }
  }
  else
  {
    program->insert_operation_node_in_dataflow(
      move_node_ptr->get_opcode(), move_node_ptr->get_operands(), lhs.get_label(), term_type);
    move_node_ptr.reset();
  }

  //}
  // else
  //{
  //  lhs.set_label(t_move.get_label());
  //}
}

template <typename T>
T &operate_copy_assignement(T &lhs, const T &rhs, ir::TermType term_type)
{

  if (lhs.get_label() == rhs.get_label())
    return lhs;

  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());

  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  std::string lhs_old_label = lhs.get_label(); // this will be used to keep track of tag
  lhs.set_new_label();
  Ptr new_lhs_node;

  bool is_lhs_output = program->type_of(lhs_old_label) == ir::ConstantTableEntryType::output;

  bool is_lhs_input = program->type_of(lhs_old_label) == ir::ConstantTableEntryType::input;

  if (program->is_tracked_object(lhs_old_label))
  {
    if (!is_lhs_input)
    {
      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), lhs_old_label);
    }

    if (is_lhs_output)
    {
      program->delete_node_from_outputs(lhs_old_label);
    }
    // program->delete_node_from_dataflow(old_label);
  }

  operate_copy(lhs, rhs, term_type);

  /*
  if (rhs_node_ptr->is_operation_node() == false)
  {
    auto const_value = program->get_entry_value_value(rhs.get_label());
    if (const_value != std::nullopt)
    {
      program->set_constant_value_value(lhs_old_label, *const_value);
      new_lhs_node = program->insert_node_in_dataflow<T>(lhs);
    }
    else
    {
      // you have no information about t_copy, for example in case it is an input ciphertext
      new_lhs_node =
        program->insert_operation_node_in_dataflow(ir::OpCode::assign, {rhs_node_ptr}, lhs.get_label(), term_type);
    }
  }
  else
  {
    new_lhs_node = program->insert_operation_node_in_dataflow(
      rhs_node_ptr->get_opcode(), rhs_node_ptr->get_operands(), lhs.get_label(), term_type);
  }
  */

  /*
    in case of assignement we need to keep/save the information about the term, this information is the content of
    constants table entry
  */

  /*
  else
  {
    lhs.set_label(rhs.get_label());
  }
  */

  return lhs;
}

template <typename T>
T &operate_move_assignement(T &lhs, T &&rhs, ir::TermType term_type)
{

  if (lhs.get_label() == rhs.get_label())
    return lhs;

  auto lhs_node_ptr = program->find_node_in_dataflow(lhs.get_label());

  auto rhs_node_ptr = program->find_node_in_dataflow(rhs.get_label());

  if (rhs_node_ptr == nullptr)
  {
    throw("operand is not defined, maybe it was only declared\n");
  }

  std::string lhs_old_label = lhs.get_label(); // this will be used to keep track of tag
  lhs.set_new_label();
  Ptr new_lhs_node;

  bool is_lhs_output = program->type_of(lhs_old_label) == ir::ConstantTableEntryType::output;

  bool is_lhs_input = program->type_of(lhs_old_label) == ir::ConstantTableEntryType::input;

  if (program->is_tracked_object(lhs_old_label))
  {

    if (!is_lhs_input)
    {
      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), lhs_old_label);
    }

    if (is_lhs_output)
      program->delete_node_from_outputs(lhs_old_label);
    // program->delete_node_from_dataflow(old_label);
  }

  operate_move<T>(lhs, std::move(rhs), term_type);

  /*
  if (rhs_node_ptr->is_operation_node() == false)
  {
    auto const_value = program->get_entry_value_value(rhs.get_label());
    if (const_value != std::nullopt)
    {
      program->set_constant_value_value(lhs.get_label(), *const_value);
      new_lhs_node = program->insert_node_in_dataflow<T>(lhs);
    }
    else
    {
      // you have no information about t_copy, for example in case it is an input ciphertext
      new_lhs_node =
        program->insert_operation_node_in_dataflow(ir::OpCode::assign, {rhs_node_ptr}, lhs.get_label(), term_type);
    }
  }
  else
  {
    std::vector<ir::Program::Ptr> operands = rhs_node_ptr->get_operands();
    new_lhs_node =
      program->insert_operation_node_in_dataflow(rhs_node_ptr->get_opcode(), operands, lhs.get_label(), term_type);
  }
  */

  /*
    in case of assignement we need to keep/save the information about the term, this information is the content of
    constants table entry
  */

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
    bool is_input = table_entry.get_entry_type() == ir::ConstantTableEntryType::input;
    if (is_output || program->is_tracked_object(old_label))
    {
      if (is_output)
        program->delete_node_from_outputs(old_label);

      if (!is_input)
      {
        program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
      }
    }
  }
  auto new_operation_node_ptr =
    program->insert_operation_node_in_dataflow(opcode, {lhs_term, rhs_term}, lhs.get_label(), term_type);
  new_operation_node_ptr->set_inplace();
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

} // namespace fhecompiler
