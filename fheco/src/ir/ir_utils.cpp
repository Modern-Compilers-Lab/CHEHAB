#include "ir_utils.hpp"
#include "fhecompiler_const.hpp"
#include <cmath>

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
    return fold_scalar_helper(lhs, rhs, add_numbers, program);
  }
  else if (opcode == ir::OpCode::mul)
  {
    return fold_scalar_helper(lhs, rhs, multiply_numbers, program);
  }
  else if (opcode == ir::OpCode::sub)
  {
    return fold_scalar_helper(lhs, rhs, subtract_numbers, program);
  }
  else
    throw("unsupported operation in fold_scalar");
}

std::shared_ptr<ir::Term> fold_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(Number &, Number, Modulus)> &e_func, Program *program)
{
  ir::Term new_scalar(ir::TermType::scalarType);
  new_scalar.set_a_default_label();

  const char *nullopt_ex_msg = "unexpected nullopt in fold_scalar_helper";

  auto lhs_const_value_opt = program->get_entry_value_value(lhs->get_label());

  if (lhs_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);

  auto rhs_const_value_opt = program->get_entry_value_value(rhs->get_label());

  if (rhs_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);

  auto lhs_const_value = *lhs_const_value_opt;
  auto rhs_const_value = *rhs_const_value_opt;

  Number lhs_value = get_constant_value_as_number(lhs_const_value);
  Number rhs_value = get_constant_value_as_number(rhs_const_value);

  e_func(lhs_value, rhs_value, program->get_plain_modulus());

  using ScalarValue = ir::ScalarValue;

  ScalarValue new_scalar_value;
  if (program->get_encryption_scheme() != fhecompiler::ckks)
  {
    int64_t to_int = static_cast<int64_t>(lhs_value);
    new_scalar_value = to_int;
  }
  else
  {
    double to_double = static_cast<double>(lhs_value);
    new_scalar_value = to_double;
  }
  ir::ConstantTableEntry::EntryValue entry_value(new_scalar.get_label(), new_scalar_value);
  ir::ConstantTableEntry entry(ir::ConstantTableEntryType::constant, entry_value);
  program->insert_entry_in_constants_table({new_scalar.get_label(), entry});
  auto new_scalar_node = std::make_shared<ir::Term>(new_scalar);
  program->insert_created_node_in_dataflow(new_scalar_node);
  return new_scalar_node;
}

Number get_constant_value_as_number(ir::ConstantValue const_value)
{
  ir::ScalarValue scalar_value = std::get<ScalarValue>(const_value);
  if (auto v = std::get_if<int64_t>(&scalar_value))
    return static_cast<Number>(*v);
  else
    return std::get<double>(scalar_value);
}

void cast_int_vector_to_number(const std::vector<int64_t> &int_vector, std::vector<Number> &double_vector)
{
  if (double_vector.size() < int_vector.size())
    double_vector.resize(int_vector.size());

  for (size_t i = 0; i < int_vector.size(); i++)
    double_vector[i] = static_cast<Number>(int_vector[i]);
}

void cast_number_vector_to_int(const std::vector<Number> &double_vector, std::vector<int64_t> &int_vector)
{
  if (int_vector.size() < double_vector.size())
    int_vector.resize(double_vector.size());

  for (size_t i = 0; i < int_vector.size(); i++)
    int_vector[i] = static_cast<int64_t>(double_vector[i]);
}

void get_constant_value_as_vector_of_number(ir::ConstantValue const_value, std::vector<Number> &double_vector)
{
  ir::VectorValue vector_value = std::get<VectorValue>(const_value);
  if (auto v = std::get_if<std::vector<double>>(&vector_value))
  {
    std::vector<Number> number_vec;
    cast_vector_of_double_to_number(number_vec, *v);
    double_vector = number_vec;
  }
  else
  {
    auto int_vector = std::get<std::vector<int64_t>>(vector_value);
    cast_int_vector_to_number(int_vector, double_vector);
  }
}

std::shared_ptr<ir::Term> fold_const_plain_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<Number> &, const std::vector<Number>, Modulus)> &e_func, Program *program)
{
  auto lhs_const_value_opt = program->get_entry_value_value(lhs->get_label());

  if (lhs_const_value_opt == std::nullopt || lhs->get_term_type() != ir::plaintextType)
  {
    throw("only plaintexts with a value at compile time are expected at fold_const_plain_helper");
  }

  auto rhs_const_value_opt = program->get_entry_value_value(rhs->get_label());

  if (rhs_const_value_opt == std::nullopt || rhs->get_term_type() != ir::plaintextType)
  {
    throw("only plaintexts with a value at compile time are expected at fold_const_plain_helper");
  }

  ir::ConstantValue lhs_const_value = *lhs_const_value_opt;
  ir::ConstantValue rhs_const_value = *rhs_const_value_opt;

  std::vector<Number> lhs_vec;
  get_constant_value_as_vector_of_number(lhs_const_value, lhs_vec);

  std::vector<Number> rhs_vec;
  get_constant_value_as_vector_of_number(rhs_const_value, rhs_vec);

  // this is not good .. better to avoid it
  if (lhs_vec.size() < rhs_vec.size())
    lhs_vec.resize(rhs_vec.size());

  e_func(lhs_vec, rhs_vec, program->get_plain_modulus());

  std::shared_ptr<ir::Term> plain_const_term = std::make_shared<ir::Term>(ir::TermType::plaintextType);
  plain_const_term->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> lhs_vec_casted;
    cast_number_vector_to_int(lhs_vec, lhs_vec_casted);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), lhs_vec_casted});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  else
  {
    std::vector<double> lhs_vec_casted;
    cast_vector_of_number_to_double(lhs_vec_casted, lhs_vec);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), lhs_vec_casted});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  program->insert_created_node_in_dataflow(plain_const_term);
  return plain_const_term;
}

std::shared_ptr<ir::Term> fold_const_plain(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, ir::OpCode opcode, ir::Program *program)
{
  switch (opcode)
  {
  case ir::OpCode::add:
    return fold_const_plain_helper(lhs, rhs, add_two_number_vectors, program);
    break;

  case ir::OpCode::mul:
    return fold_const_plain_helper(lhs, rhs, multiply_two_number_vectors, program);
    break;

  case ir::OpCode::sub:
    return fold_const_plain_helper(lhs, rhs, subtract_two_number_vectors, program);
    break;

  default:
    throw("unsuported operation in fold_const_plain");
    break;
  }
}

std::shared_ptr<ir::Term> fold_plain_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<Number> &, Number, Modulus)> &op_func, Program *program)
{
  const char *nullopt_ex_msg = "unexpected nullopt in fold_plain_scalar_helper";

  auto lhs_const_value_opt = program->get_entry_value_value(lhs->get_label());
  if (lhs_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);
  auto rhs_const_value_opt = program->get_entry_value_value(rhs->get_label());
  if (rhs_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);

  ir::ConstantValue lhs_const_value = *lhs_const_value_opt;
  ir::ConstantValue rhs_const_value = *rhs_const_value_opt;

  std::vector<Number> plain_vec;
  ir::Number scalar_value;

  if (lhs->get_term_type() == ir::plaintextType)
  {
    get_constant_value_as_vector_of_number(lhs_const_value, plain_vec);

    if (rhs->get_term_type() != ir::scalarType)
      throw("scalar expected at this stage in fold_plain_scalar_helper");

    scalar_value = get_constant_value_as_number(rhs_const_value);
  }
  else if (rhs->get_term_type() == ir::plaintextType)
  {
    get_constant_value_as_vector_of_number(rhs_const_value, plain_vec);

    if (lhs->get_term_type() != ir::scalarType)
      throw("scalar expected at this stage in fold_plain_scalar_helper");

    scalar_value = get_constant_value_as_number(lhs_const_value);
  }

  op_func(plain_vec, scalar_value, program->get_plain_modulus());

  std::shared_ptr<ir::Term> const_node = std::make_shared<ir::Term>(TermType::plaintextType);
  const_node->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> plain_vec_casted;
    cast_number_vector_to_int(plain_vec, plain_vec_casted);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {const_node->get_label(), plain_vec_casted});
    program->insert_entry_in_constants_table({const_node->get_label(), const_table_entry});
  }
  else
  {
    std::vector<double> plain_vec_casted;
    cast_vector_of_number_to_double(plain_vec_casted, plain_vec);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {const_node->get_label(), plain_vec_casted});
    program->insert_entry_in_constants_table({const_node->get_label(), const_table_entry});
  }

  program->insert_created_node_in_dataflow(const_node);

  return const_node;
}

std::shared_ptr<ir::Term> fold_plain_scalar(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs, OpCode opcode, Program *program)
{
  switch (opcode)
  {
  case OpCode::add:
    return fold_plain_scalar_helper(lhs, rhs, add_number_to_vector_of_number, program);
    break;

  case OpCode::mul:
    return fold_plain_scalar_helper(lhs, rhs, multiply_number_to_vector_of_number, program);
    break;

  case OpCode::sub:
    if (lhs->get_term_type() == ir::scalarType)
    {
      return fold_plain_scalar_helper(lhs, rhs, subtract_vector_of_number_to_number, program);
    }
    else
    {
      return fold_plain_scalar_helper(lhs, rhs, subtract_number_to_vector_of_number, program);
    }
    break;

  default:
    throw("unsupported operation in fold_plain_scalar");
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

ir::TermType deduce_ir_term_type(const std::vector<ir::Program::Ptr> &terms)
{
  if (terms.size() == 1)
    return terms[0]->get_term_type();

  if (terms.size() == 2)
    return deduce_ir_term_type(terms[0], terms[1]);

  for (size_t i = 0; i < terms.size(); i++)
  {
    if (terms[i]->get_term_type() == ir::ciphertextType)
      return ciphertextType;

    if (terms[i]->get_term_type() == ir::plaintextType)
      return ir::plaintextType;
  }
  return terms[0]->get_term_type();
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

  bool is_lhs_const = (folded_lhs->is_operation_node() == false) &&
                      (program->get_entry_value_value(folded_lhs->get_label()) != std::nullopt);

  bool is_rhs_const = (folded_rhs->is_operation_node() == false) &&
                      (program->get_entry_value_value(folded_rhs->get_label()) != std::nullopt);

  if ((folded_lhs->get_term_type() == ir::rawDataType) && (folded_rhs->get_term_type() == ir::rawDataType))
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_raw(folded_lhs, folded_rhs, term->get_opcode());
    term->replace_with(folded_term);
    return term;
  }
  else if (folded_rhs->get_term_type() == ir::scalarType && folded_lhs->get_term_type() == ir::scalarType)
  {
    if (is_lhs_const && is_rhs_const)
    {
      std::shared_ptr<ir::Term> folded_term = ir::fold_scalar(folded_lhs, folded_rhs, term->get_opcode(), program);
      term->replace_with(folded_term);
      return term;
    }
    else
      throw("scalars must be constants in fold_ir_term");
  }
  else if (folded_rhs->get_term_type() == ir::plaintextType && folded_lhs->get_term_type() == ir::plaintextType)
  {
    if (is_lhs_const && is_rhs_const)
    {
      std::shared_ptr<ir::Term> folded_term = ir::fold_const_plain(folded_lhs, folded_rhs, term->get_opcode(), program);
      term->replace_with(folded_term);
      return term;
    }
  }
  else if (is_lhs_const && is_rhs_const)
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_plain_scalar(folded_lhs, folded_rhs, term->get_opcode(), program);
    term->replace_with(folded_term);
    return term;
  }

  return term;
}

int32_t compute_depth_of(const std::shared_ptr<ir::Term> &node)
{
  return 1;
}

void print_ops_counters(Program *program)
{
  int32_t add_ct_pt(0), add_ct_ct(0), mul_ct_pt(0), mul_ct_ct(0), sub_ct_pt(0), sub_ct_ct(0), rot_ct(0);

  auto &nodes = program->get_dataflow_sorted_nodes(true);

  const char *operands_size_errormsg = "unexpected number of operands";

  auto is_pt_ct = [&operands_size_errormsg](const Program::Ptr &node) -> bool {
    auto &operands = node->get_operands();
    if (operands.size() != 2)
      throw operands_size_errormsg;

    auto &lhs = operands[0];
    auto &rhs = operands[1];

    if ((lhs->get_term_type() == ir::plaintextType) || (lhs->get_term_type() == ir::scalarType))
      return true;

    if ((rhs->get_term_type() == ir::plaintextType) || (rhs->get_term_type() == ir::scalarType))
      return true;

    return false;
  };

  for (auto &node : nodes)
  {
    if (node->is_operation_node() == false)
      continue;

    if (node->get_term_type() == ir::TermType::ciphertextType)
    {
      switch ((node->get_opcode()))
      {
      case OpCode::add:
        if (is_pt_ct(node))
          add_ct_pt++;
        else
          add_ct_ct++;
        break;

      case OpCode::mul:
        if (is_pt_ct(node))
          mul_ct_pt++;
        else
          mul_ct_ct++;
        break;

      case OpCode::sub:
        if (is_pt_ct(node))
          sub_ct_pt++;
        else
          sub_ct_ct++;
        break;

      case OpCode::rotate:
        rot_ct++;
        break;

      case OpCode::rotate_rows:
        rot_ct++;
        break;

      default:
        break;
      }
    }
    else
      continue;
  }
  std::cout << std::string(50, '-') << "\n";
  std::cout << "#(ct-ct mul) = " << mul_ct_ct << "\n";
  std::cout << "#(ct-ct add) = " << add_ct_ct << "\n";
  std::cout << "#(ct-ct sub) = " << sub_ct_ct << "\n";
  std::cout << "#(ct rotation) = " << rot_ct << "\n";
  std::cout << "#(ct-pt mul) = " << mul_ct_pt << "\n";
  std::cout << "#(ct-pt add) = " << add_ct_pt << "\n";
  std::cout << "#(ct-pt sub) = " << sub_ct_pt << "\n";
  std::cout << std::string(50, '-') << "\n";
}

ConstantTableEntryType deduce_const_table_entry_table(
  const Program::Ptr &lhs, const Program::Ptr &rhs, Program *program)
{
  ConstantTableEntryType lhs_entry_type = program->type_of(lhs->get_label());
  ConstantTableEntryType rhs_entry_type = program->type_of(rhs->get_label());

  if (lhs_entry_type == rhs_entry_type)
    return lhs_entry_type;

  else
    return ir::ConstantTableEntryType::temp;
}

void swap(Program::Ptr &lhs, Program::Ptr &rhs)
{
  Program::Ptr t = lhs;
  lhs = rhs;
  rhs = t;
}

void add_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    add_numbers(vec[i], v, modulus);
  }
}

void multiply_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    multiply_numbers(vec[i], v, modulus);
  }
}

void subtract_number_to_vector_of_number(std::vector<Number> &vec, Number v, Modulus modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    subtract_numbers(vec[i], v, modulus);
  }
}

void subtract_vector_of_number_to_number(std::vector<Number> &vec, Number v, Modulus modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    Number v_copy = v;
    subtract_numbers(v_copy, vec[i], modulus);
    vec[i] = v_copy;
  }
}

void add_numbers(Number &lhs, Number rhs, Modulus modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) + std::fmod(rhs, modulus), modulus);
}

void subtract_numbers(Number &lhs, Number rhs, Modulus modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) - std::fmod(rhs, modulus), modulus);
}

void multiply_numbers(Number &lhs, Number rhs, Modulus modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) * std::fmod(rhs, modulus), modulus);
}

void add_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    add_numbers(lhs[i], rhs[i], modulus);
  }
}

void subtract_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    subtract_numbers(lhs[i], rhs[i], modulus);
  }
}

void multiply_two_number_vectors(std::vector<Number> &lhs, const std::vector<Number> &rhs, Modulus modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    multiply_numbers(lhs[i], rhs[i], modulus);
  }
}

void negate_value_if_possible(const std::string &label, Program *program)
{
  auto node_ptr = program->find_node_in_dataflow(label);

  if (node_ptr->get_label() != label)
    throw("fatal error, node label is different than dsl object label");

  if (node_ptr->is_operation_node() == true)
    return;

  auto const_value_opt = program->get_entry_value_value(label);
  if (const_value_opt == std::nullopt)
    return;

  ir::ConstantValue const_value = *const_value_opt;

  if (node_ptr->get_term_type() == ir::plaintextType)
  {
    std::vector<Number> vec;
    get_constant_value_as_vector_of_number(const_value, vec);
    multiply_number_to_vector_of_number(vec, -1, program->get_plain_modulus());
    if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
    {
      std::vector<int64_t> vec_casted;
      cast_number_vector_to_int(vec, vec_casted);
      program->set_constant_value_value(label, vec_casted);
    }
    else
    {
      std::vector<double> vec_casted;
      cast_vector_of_number_to_double(vec_casted, vec);
      program->set_constant_value_value(label, vec_casted);
    }

    return;
  }
  else if (node_ptr->get_term_type() == ir::scalarType)
  {
    Number value;
    value = get_constant_value_as_number(const_value);
    multiply_numbers(value, -1, program->get_plain_modulus());
    if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
    {
      int64_t value_casted = static_cast<int64_t>(value);
      program->set_constant_value_value(label, value_casted);
    }
    else
    {
      double value_casted = static_cast<double>(value);
      program->set_constant_value_value(label, value_casted);
    }

    return;
  }
  throw("only scalars and plaintexts are expected in negate_value_if_possible");
}

void cast_vector_of_double_to_number(std::vector<Number> &number_vec, const std::vector<double> &double_vec)
{
  if (number_vec.size() < double_vec.size())
    number_vec.resize(double_vec.size());
  for (size_t i = 0; i < double_vec.size(); i++)
  {
    number_vec[i] = static_cast<Number>(double_vec[i]);
  }
}

void cast_vector_of_number_to_double(std::vector<double> &double_vec, const std::vector<Number> &number_vec)
{
  if (double_vec.size() < number_vec.size())
    double_vec.resize(number_vec.size());
  for (size_t i = 0; i < number_vec.size(); i++)
    double_vec[i] = static_cast<double>(number_vec[i]);
}

bool is_a_vector_of_value(const std::vector<Number> &number_vec, const ir::Number &value, size_t vector_size)
{

  if (value != 0 && number_vec.size() < vector_size)
    return false;

  for (size_t i = 0; i < number_vec.size(); i++)
    if (number_vec[i] != value)
      return false;

  return true;
}

bool check_constants_value_equality(const ConstantValue &lhs, const ConstantValue &rhs, ir::TermType term_type)
{
  if (lhs.index() != rhs.index())
    return false;
  else if (term_type == ir::plaintextType)
  {
    std::vector<ir::Number> lhs_v;
    ir::get_constant_value_as_vector_of_number(lhs, lhs_v);
    std::vector<ir::Number> rhs_v;
    ir::get_constant_value_as_vector_of_number(rhs, rhs_v);

    if (lhs_v.size() != rhs_v.size())
      return false;

    for (size_t i = 0; i < lhs_v.size(); i++)
      if (lhs_v[i] != rhs_v[i])
        return false;

    return true;
  }
  else if (term_type == ir::scalarType)
  {
    ir::Number lhs_v = ir::get_constant_value_as_number(lhs);
    ir::Number rhs_v = ir::get_constant_value_as_number(rhs);

    return lhs_v == rhs_v;
  }
  else
    throw("only scalars and plaintexts are allowed in check_constants_value_equality");
}

} // namespace ir
