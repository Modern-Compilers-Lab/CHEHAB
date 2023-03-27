#include "rewrite_rule.hpp"

namespace fheco_trs
{
size_t RewriteRule::rule_id = 0;

/*
RewriteRule::RewriteRule(const MatchingTerm &_lhs, const rhs_factory_function &_rhs_factory, bool _saving_circuit_flag)
  : lhs(_lhs), rhs_factory(_rhs_factory), id(rule_id++), saving_circuit_flag(_saving_circuit_flag)
{}
*/

/*
RewriteRule::RewriteRule(
  const MatchingTerm &_lhs, const rhs_factory_function &_rhs_factory, const MatchingTerm &_condition,
  bool _saving_circuit_flag)
  : lhs(_lhs), rhs_factory(_rhs_factory), rewrite_condition(_condition), id(rule_id++),
    saving_circuit_flag(_saving_circuit_flag)
{}
*/

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, bool _saving_circuit_flag)
  : lhs(_lhs), static_rhs(_rhs),
    rhs_factory([this](const ir::Program *, const matching_term_ir_node_map &) -> MatchingTerm { return *static_rhs; }),
    id(rule_id++), saving_circuit_flag(_saving_circuit_flag)
{}

RewriteRule::RewriteRule(
  const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition, bool _saving_circuit_flag)
  : lhs(_lhs), static_rhs(_rhs),
    rhs_factory([this](const ir::Program *, const matching_term_ir_node_map &) -> MatchingTerm { return *static_rhs; }),
    rewrite_condition(_condition), id(rule_id++), saving_circuit_flag(_saving_circuit_flag)
{}

MatchingTerm RewriteRule::get_rhs(const ir::Program *program, const matching_term_ir_node_map &matching_map) const
{
  if (static_rhs.has_value())
    return *static_rhs;
  return rhs_factory(program, matching_map);
}

bool RewriteRule::substitute_in_ir(
  std::shared_ptr<ir::Term> ir_node, core::MatchingMap &matching_map, ir::Program *program,
  core::FunctionTable &functions_table) const
{
  /*
    We call this function after ir_node is matched with lhs of rewrite rule
  */

  // saving_circuit_flag = false; //disable the check

  if (saving_circuit_flag && core::circuit_saving_condition_rewrite_rule_checker(lhs, matching_map) == false)
    return false;

  core::substitute(ir_node, rhs_factory(program, matching_map), matching_map, program, functions_table);

  return true;
}

bool RewriteRule::evaluate_rewrite_condition(
  core::MatchingMap &matching_map, ir::Program *program, core::FunctionTable &functions_table) const
{

  if (rewrite_condition == std::nullopt)
    throw("rewrite condition is not defined");

  return core::evaluate_boolean_matching_term(*rewrite_condition, matching_map, program, functions_table);
}

std::optional<core::MatchingMap> RewriteRule::match_with_ir_node(
  const ir::Program::Ptr &ir_node, ir::Program *program) const
{

  core::MatchingMap matching_map;
  bool matching_result = core::match_term(ir_node, lhs, matching_map, program);

  if (matching_result)
    return matching_map;

  return std::nullopt;
}

} // namespace fheco_trs
