#pragma once
#include "ir_utils.hpp"
#include "program.hpp"
#include <sstream>

namespace fheco_passes
{

struct SEid
{
  ir::Program::Ptr expr_ptr;
  ir::Program::Ptr lhs;
  ir::Program::Ptr rhs;
  ir::OpCode opcode = ir::OpCode::undefined;
  ir::Program *program = nullptr;

  SEid() = default;

  SEid(const ir::Program::Ptr &_lhs, const ir::Program::Ptr &_rhs, ir::OpCode _opcode)
    : lhs(_lhs), rhs(_rhs), opcode(_opcode)
  {}

  SEid(const ir::Program::Ptr &_lhs, const ir::Program::Ptr &_rhs, ir::OpCode _opcode, ir::Program *prgm)
    : lhs(_lhs), rhs(_rhs), opcode(_opcode), program(prgm)
  {}

  SEid(const ir::Program::Ptr &term_node)
  {
    if (term_node->is_operation_node())
    {
      lhs = term_node->get_operands()[0];

      if (term_node->get_operands().size() == 2)
        rhs = term_node->get_operands()[1];

      opcode = term_node->get_opcode();
    }
    expr_ptr = term_node;
  }

  SEid(const ir::Program::Ptr &term_node, ir::Program *prgm) : program(prgm)
  {
    if (term_node->is_operation_node())
    {
      lhs = term_node->get_operands()[0];

      if (term_node->get_operands().size() == 2)
        rhs = term_node->get_operands()[1];

      opcode = term_node->get_opcode();
    }
    expr_ptr = term_node;
  }

  bool operator==(const SEid &id) const
  {
    if (this->expr_ptr->get_term_type() != id.expr_ptr->get_term_type())
      return false;

    bool is_lhs_op_node = this->expr_ptr->is_operation_node();
    bool is_rhs_op_node = id.expr_ptr->is_operation_node();

    if (is_lhs_op_node != is_rhs_op_node)
      return false;

    if (is_lhs_op_node)
      return (lhs == id.lhs && rhs == id.rhs && opcode == id.opcode);

    auto lhs_const_value_opt = program->get_entry_value_value(this->expr_ptr->get_label());
    auto rhs_const_value_opt = program->get_entry_value_value(id.expr_ptr->get_label());

    if ((lhs_const_value_opt != std::nullopt) != (rhs_const_value_opt != std::nullopt))
      return false;

    if (id.expr_ptr->get_term_type() == ir::scalarType)
    {
      return ir::check_constants_value_equality(*lhs_const_value_opt, *rhs_const_value_opt, ir::scalarType);
    }

    if (id.expr_ptr->get_term_type() == ir::plaintextType && (lhs_const_value_opt != std::nullopt))
    {
      return ir::check_constants_value_equality(*lhs_const_value_opt, *rhs_const_value_opt, ir::plaintextType);
    }

    return id.expr_ptr->get_label() == this->expr_ptr->get_label(); // inputs
  }

  ~SEid() {}
};

struct SEidHash
{
  /*
  auto operator()(const SEid &seid) const -> std::string
  {
    if (seid.opcode == ir::OpCode::undefined)
    {
      return std::hash<ir::Program::Ptr>{}(seid.expr_ptr);
    }
    else
    {
      if (seid.lhs && seid.rhs) // binary operations
        return std::hash<ir::Program::Ptr>{}(seid.lhs) ^ (std::hash<size_t>{}(static_cast<size_t>(seid.opcode))) ^
               (std::hash<ir::Program::Ptr>{}(seid.rhs));
      else if (seid.lhs) // unary operations
        return std::hash<ir::Program::Ptr>{}(seid.lhs) ^ (std::hash<size_t>{}(static_cast<size_t>(seid.opcode)));
    }
  }
  */
  // This is faster than one above
  auto operator()(const SEid &seid) const -> size_t
  {
    std::stringstream ss;
    if (seid.opcode == ir::OpCode::undefined)
    {

      std::string label = seid.expr_ptr->get_label();
      ir::TermType term_type = seid.expr_ptr->get_term_type();
      auto const_value_opt = seid.program->get_entry_value_value(label);

      if (const_value_opt == std::nullopt) // this is the case for inputs
      {
        ss << label;
      }
      else
      {
        if (term_type == ir::scalarType)
        {
          ir::Number number_value = ir::get_constant_value_as_number(*const_value_opt);
          return ir::hash_number(number_value);
        }
        else
        {
          // must be a plaintext
          std::vector<ir::Number> vec_value;
          ir::get_constant_value_as_vector_of_number(*const_value_opt, vec_value);
          return ir::hash_vector_of_numbers(vec_value);
        }
      }
    }
    else
    {
      if (seid.lhs && seid.rhs) // binary operations
        ss << seid.lhs << ir::str_opcode[seid.opcode] << seid.rhs;

      else if (seid.lhs) // unary operations
        ss << ir::str_opcode[seid.opcode] << seid.lhs;
    }
    return std::hash<std::string>{}(ss.str());
  }
};

class CSE
{
private:
  ir::Program *program;

  bool check_syntactical_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs); // syntactical equality

  bool check_constants_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

  bool check_raw_data_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

  bool check_scalars_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

  bool check_plains_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

  bool check_inputs_equality(const ir::Program::Ptr &lhs, const ir::Program::Ptr &rhs);

  std::string serialize_number(const ir::Number &scalar);

  void serialize_vector_of_numbers(const std::vector<ir::Number> &numbers, std::string &s_string);

  // std::string calculate_id(const ir::Program::Ptr &term);
public:
  CSE(ir::Program *prgm) : program(prgm) {}
  // void apply_cse();

  void apply_cse2(bool allow_assign_insertion = false);
};

} // namespace fheco_passes
