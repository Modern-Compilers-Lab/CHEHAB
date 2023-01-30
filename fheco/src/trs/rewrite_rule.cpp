#include "rewrite_rule.hpp"

namespace fheco_trs
{
size_t RewriteRule::rule_id = 0;

RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs) : lhs(_lhs), rhs(_rhs), id(rule_id++) {}
RewriteRule::RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition)
  : lhs(_lhs), rhs(_rhs), rewrite_condition(_condition), id(rule_id++)
{}

} // namespace fheco_trs
