#pragma once
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

  SEid() = default;

  SEid(const ir::Program::Ptr &_lhs, const ir::Program::Ptr &_rhs, ir::OpCode _opcode)
    : lhs(_lhs), rhs(_rhs), opcode(_opcode)
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

  bool operator==(const SEid &id) const { return (lhs == id.lhs && rhs == id.rhs && opcode == id.opcode); }

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
      ss << seid.expr_ptr;
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

  // std::string calculate_id(const ir::Program::Ptr &term);

public:
  CSE(ir::Program *prgm) : program(prgm) {}
  // void apply_cse();
  /*
    This function apply CSE on the program (mainly IR). by allowing insertion of assign operator node you will gain one
    instruction at most in the best case
  */
  void apply_cse2(bool allow_assign_insertion = false);
};

} // namespace fheco_passes
