#pragma once
#include "program.hpp"

namespace fheco_passes
{

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

  std::string calculate_id(const ir::Program::Ptr &term);

public:
  CSE(ir::Program *prgm) : program(prgm) {}
  void apply_cse();
  void apply_cse2();
};

} // namespace fheco_passes
