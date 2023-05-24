#pragma once

#include "fheco/trs/subst.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <utility>

namespace fheco::ir
{
class Term;
} // namespace fheco::ir

namespace fheco::trs
{
class Rule
{
public:
  Rule(std::string label, TermMatcher lhs, TermMatcher static_rhs)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      cond_{}, valuation_{}
  {}

  Rule(std::string label, TermMatcher lhs, TermMatcher static_rhs, std::function<bool(const Subst &)> cond)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      cond_{std::move(cond)}, valuation_{}
  {}

  Rule(std::string label, TermMatcher lhs, TermMatcher static_rhs, std::function<std::int64_t(ir::Term *)> valuation)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      cond_{}, valuation_{std::move(valuation)}
  {}

  Rule(
    std::string label, TermMatcher lhs, TermMatcher static_rhs, std::function<bool(const Subst &)> cond,
    std::function<std::int64_t(ir::Term *)> valuation)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Subst &) {
        return static_rhs;
      }},
      cond_{std::move(cond)}, valuation_{std::move(valuation)}
  {}

  Rule(std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, cond_{}, valuation_{}
  {}

  Rule(
    std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs,
    std::function<std::int64_t(ir::Term *)> valuation)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, cond_{}, valuation_{std::move(valuation)}
  {}

  Rule(
    std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs,
    std::function<bool(const Subst &)> cond, std::function<std::int64_t(ir::Term *)> valuation)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, cond_{std::move(cond)},
      valuation_{std::move(valuation)}
  {}

  Rule(
    std::string label, TermMatcher lhs, std::function<TermMatcher(const Subst &)> rhs,
    std::function<bool(const Subst &)> cond)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, cond_{std::move(cond)}, valuation_{}
  {}

  inline const std::string &label() const { return label_; };

  inline const TermMatcher &lhs() const { return lhs_; };

  inline TermMatcher get_rhs(const Subst &subst = {}) const { return rhs_(subst); };

  inline const std::function<TermMatcher(const Subst &)> &rhs() const { return rhs_; }

  inline bool has_cond() const { return cond_.has_value(); }

  inline bool check_cond(const Subst &subst) const { return (*cond_)(subst); }

  inline const std::optional<std::function<bool(const Subst &)>> &cond() const { return cond_; }

  inline bool has_valuation() const { return valuation_.has_value(); }

  inline bool check_valuation(ir::Term *lhs, ir::Term *rhs) const { return (*valuation_)(lhs) > (*valuation_)(rhs); }

  inline const std::optional<std::function<std::int64_t(ir::Term *)>> &valuation() const { return valuation_; }

private:
  std::string label_;

  TermMatcher lhs_;

  std::function<TermMatcher(const Subst &)> rhs_;

  std::optional<std::function<bool(const Subst &)>> cond_;

  std::optional<std::function<std::int64_t(ir::Term *)>> valuation_;
};

inline std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
  return os << rule.label();
}
} // namespace fheco::trs
