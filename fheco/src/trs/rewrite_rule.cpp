#include "rewrite_rule.hpp"

namespace fheco_trs
{
size_t RewriteRule::rule_id = 0;

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const rhs_factory_function &_rhs_factory)
  : lhs(_lhs), rhs_factory(_rhs_factory), id(rule_id++)
{}

RewriteRule::RewriteRule(
  const MatchingTerm &_lhs, const rhs_factory_function &_rhs_factory, const MatchingTerm &_condition)
  : lhs(_lhs), rhs_factory(_rhs_factory), rewrite_condition(_condition), id(rule_id++)
{}

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs)
  : lhs(_lhs), static_rhs(_rhs), id(rule_id++)
{}

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition)
  : lhs(_lhs), static_rhs(_rhs), rewrite_condition(_condition), id(rule_id++)
{}

MatchingTerm RewriteRule::get_rhs(const ir::Program *program, const matching_term_ir_node_map &matching_map) const
{
  if (static_rhs.has_value())
    return *static_rhs;
  return rhs_factory(program, matching_map);
}

void RewriteRule::substitute_in_ir(
  std::shared_ptr<ir::Term> ir_node, core::MatchingMap &matching_map, ir::Program *program,
  core::FunctionTable &functions_table) const
{
  /*
    We call this function after ir_node is matched with lhs of rewrite rule
  */

  core::substitute(ir_node, get_rhs(program, matching_map), matching_map, program, functions_table);
}

bool RewriteRule::evaluate_rewrite_condition(
  core::MatchingMap &matching_map, ir::Program *program, core::FunctionTable &functions_table) const
{
  if (rewrite_condition == std::nullopt)
    throw("rewrite condition is not defined");

  return core::evaluate_boolean_matching_term(*rewrite_condition, matching_map, program, functions_table);
}

std::optional<core::MatchingMap> RewriteRule::match_with_ir_node(const ir::Program::Ptr &ir_node) const
{

  core::MatchingMap matching_map;
  bool matching_result = core::match_term(ir_node, lhs, matching_map);

  if (matching_result)
    return matching_map;

  return std::nullopt;
}

} // namespace fheco_trs
