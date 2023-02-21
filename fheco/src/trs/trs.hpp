#pragma once
#include "program.hpp"
#include "rewrite_rule.hpp"
#include "trs_core.hpp"
#include <optional>
#include <unordered_map>

namespace fheco_trs
{

class TRS
{
private:
  ir::Program *program;

  core::FunctionTable functions_table;

  std::vector<core::MatchingPair> apply_rule_on_ir_node(
    const std::shared_ptr<ir::Term> &ir_node, RewriteRule &rule, bool &is_rule_applied);

  void apply_rules_on_ir_node(const std::shared_ptr<ir::Term> &node, std::vector<RewriteRule> &rules);

public:
  TRS(ir::Program *prgm) : program(prgm) {}
  TRS(ir::Program *prgm, core::FunctionTable &funcs_table) : program(prgm), functions_table(funcs_table) {}
  /*
    Rules object will be modified, more specifically matching_map will be filled
  */
  void apply_rewrite_rules_on_program(std::vector<RewriteRule> &rules);
};

} // namespace fheco_trs
