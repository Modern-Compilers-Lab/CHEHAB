#pragma once

#include "fheco/trs/subst.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <functional>
#include <ostream>
#include <string>
#include <utility>

namespace fheco::trs
{
class Rule
{
public:
  Rule(
    std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs,
    std::function<bool(const Subst &)> condition = [](const Subst &) { return true; })
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, condition_{std::move(condition)}
  {}

  Rule(
    std::string label, TermMatcher lhs, TermMatcher static_rhs,
    std::function<bool(const Subst &)> condition = [](const Subst &) { return true; })
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      condition_{std::move(condition)}
  {}

  inline const std::string &label() const { return label_; };

  inline const TermMatcher &lhs() const { return lhs_; };

  inline TermMatcher get_rhs(const Subst &subst = {}) const { return rhs_(subst); };

  inline const std::function<TermMatcher(const Subst &)> &rhs() const { return rhs_; }

  inline bool check_condition(const Subst &subst = {}) const { return condition_(subst); }

  inline const std::function<bool(const Subst &)> &condition() const { return condition_; }

private:
  std::string label_;

  TermMatcher lhs_;

  std::function<TermMatcher(const Subst &)> rhs_;

  std::function<bool(const Subst &)> condition_;
};

inline std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
  return os << rule.label();
}
} // namespace fheco::trs
