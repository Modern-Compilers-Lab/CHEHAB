#include "ir_utils.hpp"

namespace ir
{

std::shared_ptr<ir::Term> fold_raw(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode)
{
  if ((lhs->get_term_type() != ir::rawDataType) || (rhs->get_term_type() != ir::rawDataType))
  {
    throw("a non rawDataType in fold_raw function");
  }
  else
  {
    int32_t lhs_value = std::stoi(lhs->get_label());
    int32_t rhs_value = std::stoi(rhs->get_label());

    int32_t folded_value;

    if (opcode == ir::OpCode::add)
      folded_value = lhs_value + rhs_value;
    else if (opcode == ir::OpCode::sub)
      folded_value = lhs_value - rhs_value;
    else if (opcode == ir::OpCode::mul)
      folded_value = lhs_value * rhs_value;
    else
      throw("unsupported operation in fold_raw");

    ir::Term new_raw_node(std::to_string(folded_value), ir::rawDataType);
    return std::make_shared<ir::Term>(new_raw_node);
  }
}

std::shared_ptr<ir::Term> fold_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program)
{
  if (lhs->get_term_type() != ir::scalarType || rhs->get_term_type() != ir::scalarType)
    throw("a non scalarType in fold_scalar");

  if (opcode == ir::OpCode::add)
  {
    return sum_scalars({lhs, rhs}, program);
  }
  else if (opcode == ir::OpCode::mul)
  {
    return multiply_scalars({lhs, rhs}, program);
  }
  else if (opcode == ir::OpCode::sub)
  {
    return subtract_scalars({lhs, rhs}, program);
  }
  else
    throw("unsupported operation in fold_scalar");
}

std::shared_ptr<ir::Term> multiply_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program)
{
  if (scalars.size() == 1)
    return scalars[0];

  ir::Term new_scalar(ir::TermType::scalarType);
  new_scalar.set_a_default_label();

  using ScalarValue = ir::ScalarValue;

  double product = 1;
  for (auto &scalar : scalars)
  {
    auto table_entry = program->get_entry_form_constants_table(scalar->get_label());
    if (table_entry == std::nullopt)
    {
      throw("unexpected, scalar don't exist in constants table");
    }
    else
    {
      ScalarValue scalar_value = std::get<ScalarValue>(*(*table_entry).get().get_entry_value().value);
      if (auto double_value = std::get_if<double>(&scalar_value))
      {
        product *= (*double_value);
      }
      else
      {
        int64_t int_value = std::get<int64_t>(scalar_value);
        product *= int_value;
      }
    }
  }
  ScalarValue new_scalar_value;
  if (program->get_encryption_scheme() != fhecompiler::ckks)
  {
    int64_t to_int = product;
    new_scalar_value = to_int;
  }
  else
  {
    new_scalar_value = product;
  }
  ir::ConstantTableEntry::EntryValue entry_value(new_scalar.get_label(), new_scalar_value);
  ir::ConstantTableEntry entry(ir::ConstantTableEntryType::constant, entry_value);
  program->insert_entry_in_constants_table({new_scalar.get_label(), entry});

  return std::make_shared<ir::Term>(new_scalar);
}

std::shared_ptr<ir::Term> sum_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program)
{
  ir::Term new_scalar(ir::TermType::scalarType);
  new_scalar.set_a_default_label();

  using ScalarValue = ir::ScalarValue;

  double sum = 0;
  for (auto &scalar : scalars)
  {
    auto table_entry = program->get_entry_form_constants_table(scalar->get_label());
    if (table_entry == std::nullopt)
    {
      throw("unexpected, scalar don't exist in constants table");
    }
    else
    {
      ScalarValue scalar_value = std::get<ScalarValue>(*(*table_entry).get().get_entry_value().value);
      if (auto double_value = std::get_if<double>(&scalar_value))
      {
        sum += (*double_value);
      }
      else
      {
        int64_t int_value = std::get<int64_t>(scalar_value);
        sum += int_value;
      }
    }
  }
  ScalarValue new_scalar_value;
  if (program->get_encryption_scheme() != fhecompiler::ckks)
  {
    int64_t to_int = sum;
    new_scalar_value = to_int;
  }
  else
  {
    new_scalar_value = sum;
  }
  ir::ConstantTableEntry::EntryValue entry_value(new_scalar.get_label(), new_scalar_value);
  ir::ConstantTableEntry entry(ir::ConstantTableEntryType::constant, entry_value);
  program->insert_entry_in_constants_table({new_scalar.get_label(), entry});

  return std::make_shared<ir::Term>(new_scalar);
}

std::shared_ptr<ir::Term> subtract_scalars(const std::vector<std::shared_ptr<ir::Term>> &scalars, ir::Program *program)
{
  ir::Term new_scalar(ir::TermType::scalarType);
  new_scalar.set_a_default_label();

  using ScalarValue = ir::ScalarValue;

  double subtraction = 0;
  for (size_t i = 0; i < scalars.size(); i++)
  {
    auto scalar = scalars[i];
    auto table_entry = program->get_entry_form_constants_table(scalar->get_label());
    if (table_entry == std::nullopt)
    {
      throw("unexpected, scalar don't exist in constants table");
    }
    else
    {
      ScalarValue scalar_value = std::get<ScalarValue>(*(*table_entry).get().get_entry_value().value);
      if (auto double_value = std::get_if<double>(&scalar_value))
      {
        if (i == 0)
          subtraction = (*double_value);
        else
          subtraction -= (*double_value);
      }
      else
      {
        int64_t int_value = std::get<int64_t>(scalar_value);
        if (i == 0)
          subtraction = int_value;
        else
          subtraction -= int_value;
      }
    }
  }
  ScalarValue new_scalar_value;
  if (program->get_encryption_scheme() != fhecompiler::ckks)
  {
    int64_t to_int = subtraction;
    new_scalar_value = to_int;
  }
  else
  {
    new_scalar_value = subtraction;
  }
  ir::ConstantTableEntry::EntryValue entry_value(new_scalar.get_label(), new_scalar_value);
  ir::ConstantTableEntry entry(ir::ConstantTableEntryType::constant, entry_value);
  program->insert_entry_in_constants_table({new_scalar.get_label(), entry});

  return std::make_shared<ir::Term>(new_scalar);
}

double get_constant_value_as_double(ir::ConstantValue const_value)
{
  ir::ScalarValue scalar_value = std::get<ScalarValue>(const_value);
  if (auto v = std::get_if<int64_t>(&scalar_value))
    return static_cast<double>(*v);
  else
    return std::get<double>(scalar_value);
}

} // namespace ir
