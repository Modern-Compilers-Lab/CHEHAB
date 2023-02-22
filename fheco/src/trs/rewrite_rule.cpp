#include "rewrite_rule.hpp"

namespace fheco_trs
{
size_t RewriteRule::rule_id = 0;

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs) : lhs(_lhs), rhs(_rhs), id(rule_id++) {}

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition)
  : lhs(_lhs), rhs(_rhs), rewrite_condition(_condition), id(rule_id++)
{}

void RewriteRule::substitute_in_ir(
  std::shared_ptr<ir::Term> ir_node, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
  core::FunctionTable &functions_table) const
{
  /*
    We call this function after ir_node is matched with lhs of rewrite rule
  */
  core::substitute(ir_node, rhs, matching_map, program, functions_table);
}

bool RewriteRule::evaluate_rewrite_condition(
  std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
  core::FunctionTable &functions_table) const
{
  if (rewrite_condition == std::nullopt)
    throw("rewrite condition is not defined");

  return core::evaluate_boolean_matching_term(*rewrite_condition, matching_map, program, functions_table);
}

} // namespace fheco_trs
