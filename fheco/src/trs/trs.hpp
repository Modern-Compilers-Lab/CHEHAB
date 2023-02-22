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
  static core::FunctionTable functions_table;

  std::vector<core::MatchingPair> apply_rule_on_ir_node(
    const std::shared_ptr<ir::Term> &ir_node, const RewriteRule &rule, bool &is_rule_applied);

  void apply_rules_on_ir_node(const std::shared_ptr<ir::Term> &node, const std::vector<RewriteRule> &rules);

public:
  TRS(ir::Program *prgm) : program(prgm) {}

  void apply_rewrite_rules_on_program(const std::vector<RewriteRule> &rules);
};

} // namespace fheco_trs
