#pragma once

#include "fheco/trs/common.hpp"
#include "fheco/trs/subst.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::trs
{
class Rule
{
public:
  static std::function<bool(const Subst &)> is_not_const(TermMatcher x, std::shared_ptr<ir::Func> func);

  static std::function<bool(const Subst &)> is_not_rotation(TermMatcher x);

  static std::function<bool(const Subst &)> has_less_ctxt_leaves(TermMatcher x, TermMatcher y, TermsMetric &dp);

  static std::function<bool(const Subst &)> has_less_ctxt_leaves(
    TermMatcher x, TermMatcher y1, TermMatcher y2, TermsMetric &dp);

  Rule(
    std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs,
    std::function<bool(const Subst &)> cond = [](const Subst &) { return true; })
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, cond_{std::move(cond)}
  {}

  Rule(
    std::string label, TermMatcher lhs, TermMatcher static_rhs,
    std::function<bool(const Subst &)> cond = [](const Subst &) { return true; })
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      cond_{std::move(cond)}
  {}

  inline const std::string &label() const { return label_; };

  inline const TermMatcher &lhs() const { return lhs_; };

  inline TermMatcher get_rhs(const Subst &subst = {}) const { return rhs_(subst); };

  inline const std::function<TermMatcher(const Subst &)> &rhs() const { return rhs_; }

  inline bool check_cond(const Subst &subst = {}) const { return cond_(subst); }

  inline const std::function<bool(const Subst &)> &cond() const { return cond_; }

private:
  std::string label_;

  TermMatcher lhs_;

  std::function<TermMatcher(const Subst &)> rhs_;

  std::function<bool(const Subst &)> cond_;
};

inline std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
  return os << rule.label();
}
} // namespace fheco::trs
