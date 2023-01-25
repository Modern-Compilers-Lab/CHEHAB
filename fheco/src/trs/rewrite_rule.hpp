#pragma once
#include "matching_term.hpp"
#include <optional>

namespace fheco_trs
{

class RewriteRule
{
private:
  MatchingTerm lhs;
  MatchingTerm rhs;
  std::optional<MatchingTerm> rewrite_condition;
  /* by definition all variable in rhs must be in lhs */
public:
  RewriteRule() = delete;
  RewriteRule(const RewriteRule &) = default;
  RewriteRule(RewriteRule &&) = default;
  RewriteRule &operator=(const RewriteRule &) = default;
  RewriteRule &operator=(RewriteRule &&) = default;

  RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs) : lhs(_lhs), rhs(_rhs) {}

  RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition)
    : lhs(_lhs), rhs(_rhs), rewrite_condition(_condition)
  {}

  const MatchingTerm &get_lhs() const { return lhs; }
  const MatchingTerm &get_rhs() const { return rhs; }
  const std::optional<MatchingTerm> &get_rewrite_condition() const { return rewrite_condition; }

  ~RewriteRule() {}
};

} // namespace fheco_trs
