#pragma once
#include "program.hpp"

namespace fheco_passes
{

class RotationRewritePass
{
private:
  ir::Program *program;
  static size_t pass_instruction_id;
  const std::string inst_keyword = "rotation_pass";

  void apply_rule1(const std::shared_ptr<ir::Term> &node_term);
  void apply_rule2(const std::shared_ptr<ir::Term> &node_term);
  void apply_rule3(const std::shared_ptr<ir::Term> &node_term);

public:
  RotationRewritePass(ir::Program *prgm) : program(prgm) {}
  void apply_rewrite();
};

} // namespace fheco_passes
