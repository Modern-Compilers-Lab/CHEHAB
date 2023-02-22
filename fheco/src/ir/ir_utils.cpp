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
      throw("unexpected, scalar doesnt exist in constants table");
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
      throw("unexpected, scalar doesnt exist in constants table");
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
      throw("unexpected, scalar doesnt exist in constants table");
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

void cast_int_vector_to_double(const std::vector<int64_t> &int_vector, std::vector<double> &double_vector)
{
  if (double_vector.size() < int_vector.size())
    double_vector.resize(int_vector.size());

  for (size_t i = 0; i < int_vector.size(); i++)
    double_vector[i] = static_cast<double>(int_vector[i]);
}

void cast_double_vector_to_int(const std::vector<double> &double_vector, std::vector<int64_t> &int_vector)
{
  if (int_vector.size() < double_vector.size())
    int_vector.resize(double_vector.size());

  for (size_t i = 0; i < int_vector.size(); i++)
    int_vector[i] = static_cast<int64_t>(double_vector[i]);
}

void get_constant_value_as_vector_of_double(ir::ConstantValue const_value, std::vector<double> &double_vector)
{
  ir::VectorValue vector_value = std::get<VectorValue>(const_value);
  if (auto v = std::get_if<std::vector<double>>(&vector_value))
    double_vector = *v;
  else
  {
    auto int_vector = std::get<std::vector<int64_t>>(vector_value);
    cast_int_vector_to_double(int_vector, double_vector);
  }
}

std::shared_ptr<ir::Term> sum_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program)
{

  if (const_plains.size() == 0)
    throw("empty const_plains vector in sum_const_plains");

  if (const_plains.size() == 1)
    return const_plains[0];

  std::vector<double> const_plains_sum; // simd sum
  for (size_t i = 0; i < const_plains.size(); i++)
  {
    auto &const_plain = const_plains[i];
    if (
      program->type_of(const_plain->get_label()) != ir::ConstantTableEntryType::constant ||
      const_plain->get_term_type() != ir::plaintextType)
    {
      throw("only constant plaintext are expected at sum_const_plains");
    }

    ir::ConstantValue const_value =
      *(*program->get_entry_form_constants_table(const_plain->get_label())).get().get_entry_value().value;

    std::vector<double> vector_value;
    get_constant_value_as_vector_of_double(const_value, vector_value);
    if (i == 0)
      const_plains_sum = vector_value;
    else
    {
      // this is bad and better to avoid it
      if (const_plains_sum.size() < vector_value.size())
        const_plains_sum.resize(vector_value.size());

      for (size_t j = 0; j < vector_value.size(); j++)
        const_plains_sum[j] += vector_value[j];
    }
  }
  std::shared_ptr<ir::Term> plain_const_term = std::make_shared<ir::Term>(ir::TermType::plaintextType);
  plain_const_term->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> const_plains_sum_int;
    cast_double_vector_to_int(const_plains_sum, const_plains_sum_int);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), const_plains_sum_int});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  else
  {
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), const_plains_sum});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  return plain_const_term;
}

std::shared_ptr<ir::Term> subtract_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program)
{

  if (const_plains.size() == 0)
    throw("empty const_plains vector in subtract_const_plains");

  if (const_plains.size() == 1)
    return const_plains[0];

  std::vector<double> const_plains_sub; // simd subtraction
  for (size_t i = 0; i < const_plains.size(); i++)
  {
    auto &const_plain = const_plains[i];
    if (
      program->type_of(const_plain->get_label()) != ir::ConstantTableEntryType::constant ||
      const_plain->get_term_type() != ir::plaintextType)
    {
      throw("only constant plaintext are expected at subtract_const_plains");
    }

    ir::ConstantValue const_value =
      *(*program->get_entry_form_constants_table(const_plain->get_label())).get().get_entry_value().value;

    std::vector<double> vector_value;
    get_constant_value_as_vector_of_double(const_value, vector_value);
    if (i == 0)
      const_plains_sub = vector_value;
    else
    {
      // this is bad and better to avoid it
      if (const_plains_sub.size() < vector_value.size())
        const_plains_sub.resize(vector_value.size());

      for (size_t j = 0; j < vector_value.size(); j++)
        const_plains_sub[j] -= vector_value[j];
    }
  }
  std::shared_ptr<ir::Term> plain_const_term = std::make_shared<ir::Term>(ir::TermType::plaintextType);
  plain_const_term->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> const_plains_sub_int;
    cast_double_vector_to_int(const_plains_sub, const_plains_sub_int);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), const_plains_sub_int});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  else
  {
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), const_plains_sub});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  return plain_const_term;
}

std::shared_ptr<ir::Term> multiply_const_plains(
  const std::vector<std::shared_ptr<ir::Term>> &const_plains, ir::Program *program)
{

  if (const_plains.size() == 0)
    throw("empty const_plains vector multiply_const_plains");

  if (const_plains.size() == 1)
    return const_plains[0];

  std::vector<double> const_plains_product; // simd product
  for (size_t i = 0; i < const_plains.size(); i++)
  {
    auto &const_plain = const_plains[i];
    if (
      program->type_of(const_plain->get_label()) != ir::ConstantTableEntryType::constant ||
      const_plain->get_term_type() != ir::plaintextType)
    {
      throw("only constant plaintext are expected at multiply_const_plains");
    }

    ir::ConstantValue const_value =
      *(*program->get_entry_form_constants_table(const_plain->get_label())).get().get_entry_value().value;

    std::vector<double> vector_value;
    get_constant_value_as_vector_of_double(const_value, vector_value);
    if (i == 0)
      const_plains_product = vector_value;
    else
    {
      // this is bad and better to avoid it
      if (const_plains_product.size() < vector_value.size())
        const_plains_product.resize(vector_value.size());

      for (size_t j = 0; j < vector_value.size(); j++)
        const_plains_product[j] *= vector_value[j];
    }
  }

  std::shared_ptr<ir::Term> plain_const_term = std::make_shared<ir::Term>(ir::TermType::plaintextType);
  plain_const_term->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> cconst_plains_product_int;
    cast_double_vector_to_int(const_plains_product, cconst_plains_product_int);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), cconst_plains_product_int});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  else
  {
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), const_plains_product});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  return plain_const_term;
}

std::shared_ptr<ir::Term> fold_const_plain(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program)
{
  switch (opcode)
  {
  case ir::OpCode::add:
    return sum_const_plains({lhs, rhs}, program);
    break;

  case ir::OpCode::mul:
    return multiply_const_plains({lhs, rhs}, program);
    break;

  case ir::OpCode::sub:
    return subtract_const_plains({lhs, rhs}, program);
    break;

  default:
    throw("unsuported operation in fold_const_plain");
    break;
  }
}

int32_t get_rotation_step(const std::shared_ptr<ir::Term> &node)
{
  if (node->get_operands()[0]->get_term_type() == ir::rawDataType)
  {
    return std::stoi(node->get_operands()[0]->get_label());
  }
  else if (node->get_operands()[1]->get_term_type() == ir::rawDataType)
  {
    return std::stoi(node->get_operands()[1]->get_label());
  }
  throw("invalid rotation node in get_rotation_step");
}

ir::TermType deduce_ir_term_type(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs)
{
  if (lhs->get_term_type() == rhs->get_term_type())
    return lhs->get_term_type();

  if (lhs->get_term_type() == ir::ciphertextType || rhs->get_term_type() == ir::ciphertextType)
    return ir::ciphertextType;

  if (lhs->get_term_type() == ir::plaintextType || rhs->get_term_type() == ir::plaintextType)
    return ir::plaintextType;

  throw("couldn't deduce ir term type");
}

std::shared_ptr<ir::Term> fold_ir_term(const std::shared_ptr<ir::Term> &term, ir::Program *program)
{

  if (term->is_operation_node() == false)
    return term;

  if (term->get_term_type() == ir::ciphertextType)
    return term;

  auto operands = term->get_operands();
  if (operands.size() != 2) // only fold binary
    return term;

  // fold in depth
  std::shared_ptr<ir::Term> folded_lhs = fold_ir_term(operands[0], program);
  std::shared_ptr<ir::Term> folded_rhs = fold_ir_term(operands[1], program);

  if ((folded_lhs->get_term_type() == ir::rawDataType) && (folded_rhs->get_term_type() == ir::rawDataType))
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_raw(folded_lhs, folded_rhs, term->get_opcode());
    term->replace_with(folded_term);
    return term;
  }

  if (folded_rhs->get_term_type() == ir::scalarType && folded_lhs->get_term_type() == ir::scalarType)
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_scalar(folded_lhs, folded_rhs, term->get_opcode(), program);
    term->replace_with(folded_term);
    return term;
  }

  bool is_lhs_a_const_plain = (folded_lhs->get_term_type() == ir::plaintextType) &&
                              (program->type_of(folded_lhs->get_label()) == ir::ConstantTableEntryType::constant);

  bool is_rhs_a_const_plain = (folded_rhs->get_term_type() == ir::plaintextType) &&
                              (program->type_of(folded_rhs->get_label()) == ir::ConstantTableEntryType::constant);

  if (is_lhs_a_const_plain && is_rhs_a_const_plain)
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_const_plain(folded_lhs, folded_rhs, term->get_opcode(), program);
    term->replace_with(folded_term);
    return term;
  }

  return term;
}

int32_t compute_depth_of(const std::shared_ptr<ir::Term> &node)
{
  return 1;
}

} // namespace ir
