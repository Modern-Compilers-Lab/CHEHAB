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
  : lhs(_lhs), static_rhs(_rhs),
    rhs_factory([this](const ir::Program *, const matching_term_ir_node_map &) -> MatchingTerm { return *static_rhs; }),
    id(rule_id++)
{}

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition)
  : lhs(_lhs), static_rhs(_rhs),
    rhs_factory([this](const ir::Program *, const matching_term_ir_node_map &) -> MatchingTerm { return *static_rhs; }),
    rewrite_condition(_condition), id(rule_id++)
{}

MatchingTerm RewriteRule::get_rhs(const ir::Program *program, const matching_term_ir_node_map &matching_map) const
{
  return rhs_factory(program, matching_map);
}

} // namespace fheco_trs
