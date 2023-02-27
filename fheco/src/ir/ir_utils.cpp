#include "ir_utils.hpp"
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
    return fold_scalar_helper(lhs, rhs, add_scalars, program);
  }
  else if (opcode == ir::OpCode::mul)
  {
    return fold_scalar_helper(lhs, rhs, multiply_scalars, program);
  }
  else if (opcode == ir::OpCode::sub)
  {
    return fold_scalar_helper(lhs, rhs, subtract_scalars, program);
  }
  else
    throw("unsupported operation in fold_scalar");
}

std::shared_ptr<ir::Term> fold_scalar_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(double &, double, int64_t)> &e_func, Program *program)
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

  double lhs_value = get_constant_value_as_double(lhs_const_value);
  double rhs_value = get_constant_value_as_double(rhs_const_value);

  e_func(lhs_value, rhs_value, program->get_plain_modulus());

  using ScalarValue = ir::ScalarValue;

  ScalarValue new_scalar_value;
  if (program->get_encryption_scheme() != fhecompiler::ckks)
  {
    int64_t to_int = lhs_value;
    new_scalar_value = to_int;
  }
  else
  {
    new_scalar_value = lhs_value;
  }
  ir::ConstantTableEntry::EntryValue entry_value(new_scalar.get_label(), new_scalar_value);
  ir::ConstantTableEntry entry(ir::ConstantTableEntryType::constant, entry_value);
  program->insert_entry_in_constants_table({new_scalar.get_label(), entry});
  auto new_scalar_node = std::make_shared<ir::Term>(new_scalar);
  program->insert_created_node_in_dataflow(new_scalar_node);
  return new_scalar_node;
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

std::shared_ptr<ir::Term> fold_const_plain_helper(
  const std::shared_ptr<ir::Term> &lhs, const std::shared_ptr<ir::Term> &rhs,
  const std::function<void(std::vector<double> &, const std::vector<double>, int64_t)> &e_func, Program *program)
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

  std::vector<double> lhs_vec;
  get_constant_value_as_vector_of_double(lhs_const_value, lhs_vec);

  std::vector<double> rhs_vec;
  get_constant_value_as_vector_of_double(rhs_const_value, rhs_vec);

  // this is not good .. better to avoid it
  if (lhs_vec.size() < rhs_vec.size())
    lhs_vec.resize(rhs_vec.size());

  e_func(lhs_vec, rhs_vec, program->get_plain_modulus());

  std::shared_ptr<ir::Term> plain_const_term = std::make_shared<ir::Term>(ir::TermType::plaintextType);
  plain_const_term->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> lhs_vec_casted;
    cast_double_vector_to_int(lhs_vec, lhs_vec_casted);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), lhs_vec_casted});
    program->insert_entry_in_constants_table({plain_const_term->get_label(), const_table_entry});
  }
  else
  {
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {plain_const_term->get_label(), lhs_vec});
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
    return fold_const_plain_helper(lhs, rhs, add_two_double_vectors, program);
    break;

  case ir::OpCode::mul:
    return fold_const_plain_helper(lhs, rhs, multiply_two_double_vectors, program);
    break;

  case ir::OpCode::sub:
    return fold_const_plain_helper(lhs, rhs, subtract_two_double_vectors, program);
    break;

  default:
    throw("unsuported operation in fold_const_plain");
    break;
  }
}

std::shared_ptr<ir::Term> fold_plain_scalar_helper(
  const std::shared_ptr<ir::Term> &_lhs, const std::shared_ptr<ir::Term> &_rhs,
  const std::function<void(std::vector<double> &, double, int64_t)> &op_func, Program *program)
{
  const char *nullopt_ex_msg = "unexpected nullopt in fold_plain_scalar_helper";

  std::shared_ptr<ir::Term> lhs(_lhs), rhs(_rhs);

  if (_lhs->get_term_type() == ir::scalarType)
  {
    lhs = _rhs;
    rhs = _lhs;
  }

  auto plain_const_value_opt = program->get_entry_value_value(lhs->get_label());
  if (plain_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);
  auto scalar_const_value_opt = program->get_entry_value_value(rhs->get_label());
  if (scalar_const_value_opt == std::nullopt)
    throw(nullopt_ex_msg);

  ir::ConstantValue plain_const_value = *plain_const_value_opt;
  ir::ConstantValue scalar_const_value = *scalar_const_value_opt;

  std::vector<double> plain_vec;
  get_constant_value_as_vector_of_double(plain_const_value, plain_vec);
  double scalar_value = get_constant_value_as_double(scalar_const_value);

  op_func(plain_vec, scalar_value, program->get_plain_modulus());

  std::shared_ptr<ir::Term> const_node = std::make_shared<ir::Term>(TermType::plaintextType);
  const_node->set_a_default_label();

  if (program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
  {
    std::vector<int64_t> plain_vec_casted;
    cast_double_vector_to_int(plain_vec, plain_vec_casted);
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {const_node->get_label(), plain_vec_casted});
    program->insert_entry_in_constants_table({const_node->get_label(), const_table_entry});
  }
  else
  {
    ir::ConstantTableEntry const_table_entry(
      ir::ConstantTableEntryType::constant, {const_node->get_label(), plain_vec});
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
    return fold_plain_scalar_helper(lhs, rhs, add_double_to_vector_of_double, program);
    break;

  case OpCode::mul:
    return fold_plain_scalar_helper(lhs, rhs, multiply_double_to_vector_of_double, program);
    break;

  case OpCode::sub:
    return fold_plain_scalar_helper(lhs, rhs, subtract_double_to_vector_of_double, program);
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

      case OpCode::rotate_rows:
        rot_ct++;
        break;
      case OpCode::rotate:
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

void add_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    add_scalars(vec[i], v, modulus);
  }
}

void multiply_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    multiply_scalars(vec[i], v, modulus);
  }
}

void subtract_double_to_vector_of_double(std::vector<double> &vec, double v, int64_t modulus)
{
  for (size_t i = 0; i < vec.size(); i++)
  {
    subtract_scalars(vec[i], v, modulus);
  }
}

void add_scalars(double &lhs, double rhs, int64_t modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) + std::fmod(rhs, modulus), modulus);
}

void subtract_scalars(double &lhs, double rhs, int64_t modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) - std::fmod(rhs, modulus), modulus);
}

void multiply_scalars(double &lhs, double rhs, int64_t modulus)
{
  lhs = std::fmod(std::fmod(lhs, modulus) * std::fmod(rhs, modulus), modulus);
}

void add_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    add_scalars(lhs[i], rhs[i], modulus);
  }
}

void subtract_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    subtract_scalars(lhs[i], rhs[i], modulus);
  }
}

void multiply_two_double_vectors(std::vector<double> &lhs, const std::vector<double> &rhs, int64_t modulus)
{
  for (size_t i = 0; i < lhs.size(); i++)
  {
    multiply_scalars(lhs[i], rhs[i], modulus);
  }
}

} // namespace ir
