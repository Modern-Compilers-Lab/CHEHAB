#include "arith_expr_pass.hpp"
#include "scalar.hpp"

using namespace fhecompiler;

namespace fheco_passes
{

ir::Program::Ptr ArithmeticExpressionSimplfier::make_scalar_term(int64_t value)
{
  Scalar scalar_term(value);
  ir::Program::Ptr node = program->find_node_in_dataflow(scalar_term.get_label());
  if (node == nullptr)
    throw("scalar node not inserted in make_scalar_term function");
  return node;
}

ir::Program::Ptr ArithmeticExpressionSimplfier::make_scalar_term(double value)
{
  Scalar scalar_term(value);
  ir::Program::Ptr node = program->find_node_in_dataflow(scalar_term.get_label());
  if (node == nullptr)
    throw("scalar node not inserted in make_scalar_term function");
  return node;
}

bool ArithmeticExpressionSimplfier::convert_difference_to_sum(const ir::Program::Ptr &node)
{
  if (node->is_operation_node() == false)
    return false;

  if (
    node->get_opcode() != ir::OpCode::sub_plain && node->get_opcode() != ir::OpCode::sub &&
    node->get_opcode() != ir::OpCode::negate)
    return false;

  /*
    At this level it is certain that we have one type of subtraction, either sub or sub_plain.
    Rewrite rule is the following : a - b = a + (-1)*b && -a = (-1)*a
  */

  Scheme scheme_type = program->get_encryption_scheme();

  ir::Program::Ptr minus_one_term =
    make_scalar_term(scheme_type == Scheme::ckks ? static_cast<double>(-1) : static_cast<int64_t>(-1));

  if (node->get_opcode() == ir::OpCode::negate)
  {
    if (node->get_operands().size() != 1)
      throw("unexpected number of operands in negate operation in function convert_difference_to_sum");

    ir::Program::Ptr operand = node->get_operands()[0];

    node->set_opcode(ir::OpCode::mul_plain);
    node->add_operand(minus_one_term);
  }
  else
  {
    if (node->get_operands().size() != 2)
      throw("unexpected number of operands in convert_difference_to_sum function");

    ir::Program::Ptr lhs = node->get_operands()[0];
    ir::Program::Ptr rhs = node->get_operands()[1];

    if (node->get_opcode() == ir::OpCode::sub_plain)
      node->set_opcode(ir::OpCode::add_plain);
    else
      node->set_opcode(ir::OpCode::add);

    ir::Program::Ptr new_rhs = program->insert_operation_node_in_dataflow(
      ir::OpCode::mul_plain, std::vector<ir::Program::Ptr>({minus_one_term, rhs}), "", rhs->get_term_type());
    new_rhs->set_a_default_label();

    node->delete_operand_at_index(1);
    node->add_operand(new_rhs);
  }

  return true;
}

bool ArithmeticExpressionSimplfier::compact_sum(const ir::Program::Ptr &node) {}

bool ArithmeticExpressionSimplfier::compact_product(const ir::Program::Ptr &node) {}

bool ArithmeticExpressionSimplfier::consolidate_product(const ir::Program::Ptr &node) {}

bool ArithmeticExpressionSimplfier::simplify_product(const ir::Program::Ptr &node) {}

bool ArithmeticExpressionSimplfier::simplify_sum(const ir::Program::Ptr &node) {}

bool ArithmeticExpressionSimplfier::simplify_constants(const ir::Program::Ptr &node) {}

void ArithmeticExpressionSimplfier::apply_rules()
{
  auto nodes_sorted = program->get_dataflow_sorted_nodes(true);

  for (auto &node : nodes_sorted)
  {
    convert_difference_to_sum(node);
    compact_sum(node);
    compact_product(node);
    consolidate_product(node);
    simplify_sum(node);
    simplify_constants(node);
  }
}

} // namespace fheco_passes
