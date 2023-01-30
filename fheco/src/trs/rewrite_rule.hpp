#pragma once
#include "matching_term.hpp"
#include <optional>

namespace fheco_trs
{

class RewriteRule
{
private:
  static size_t rule_id;
  size_t id;
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

  RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs);

  RewriteRule(const MatchingTerm &_lhs, const MatchingTerm &_rhs, const MatchingTerm &_condition);

  size_t get_id() const { return id; }
  const MatchingTerm &get_lhs() const { return lhs; }
  const MatchingTerm &get_rhs() const { return rhs; }
  const std::optional<MatchingTerm> &get_rewrite_condition() const { return rewrite_condition; }

  ~RewriteRule() {}
};

} // namespace fheco_trs
