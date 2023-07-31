#pragma once

#include "fheco/trs/common.hpp"
#include "fheco/trs/substitution.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::trs
{
class Rule
{
public:
  static std::function<bool(const Substitution &)> is_not_const(TermMatcher x, std::shared_ptr<ir::Func> func);

  static std::function<bool(const Substitution &)> is_not_rotation(TermMatcher x);

  static std::function<bool(const Substitution &)> has_less_ctxt_leaves(TermMatcher x, TermMatcher y, TermsMetric &cache);

  static std::function<bool(const Substitution &)> has_less_ctxt_leaves(
    TermMatcher x, TermMatcher y1, TermMatcher y2, TermsMetric &cache);

  Rule(std::string name, TermMatcher lhs, TermMatcher static_rhs)
    : name_{std::move(name)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Substitution &) {
        return static_rhs;
      }},
      has_dynamic_rhs_{false}, cond_{[](const Substitution &) {
        return true;
      }},
      has_cond_{false}
  {}

  Rule() : Rule("", TermMatcher{}, TermMatcher{}) {}

  Rule(std::string name, TermMatcher lhs, TermMatcher static_rhs, std::function<bool(const Substitution &)> cond)
    : name_{std::move(name)}, lhs_{std::move(lhs)}, rhs_{[static_rhs = std::move(static_rhs)](const Substitution &) {
        return static_rhs;
      }},
      has_dynamic_rhs_{false}, cond_{std::move(cond)}, has_cond_{true}
  {}

  Rule(std::string name, TermMatcher lhs, std::function<TermMatcher(const Substitution &)> rhs)
    : name_{std::move(name)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, has_dynamic_rhs_{true},
      cond_{[](const Substitution &) {
        return true;
      }},
      has_cond_{false}
  {}

  Rule(
    std::string name, TermMatcher lhs, std::function<TermMatcher(const Substitution &)> rhs,
    std::function<bool(const Substitution &)> cond)
    : name_{std::move(name)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, has_dynamic_rhs_{true},
      cond_{std::move(cond)}, has_cond_{true}
  {}

  std::vector<Rule> generate_customized_terms_variants() const;

  inline TermMatcher::RefWrappSet get_variables() const { return lhs_.get_variables(); }

  inline TermMatcher::RefWrappSet get_generic_variables() const { return lhs_.get_generic_variables(); }

  inline const std::string &name() const { return name_; };

  inline const TermMatcher &lhs() const { return lhs_; };

  inline TermMatcher get_rhs(const Substitution &subst = {}) const { return rhs_(subst); };

  inline const std::function<TermMatcher(const Substitution &)> &rhs() const { return rhs_; }

  inline bool has_dynamic_rhs() const { return has_dynamic_rhs_; }

  inline bool check_cond(const Substitution &subst = {}) const { return cond_(subst); }

  inline const std::function<bool(const Substitution &)> &cond() const { return cond_; }

  inline bool has_cond() const { return has_cond_; }

private:
  std::string name_;

  TermMatcher lhs_;

  std::function<TermMatcher(const Substitution &)> rhs_;

  bool has_dynamic_rhs_;

  std::function<bool(const Substitution &)> cond_;

  bool has_cond_;
};

inline bool operator==(const Rule &lhs, const Rule &rhs)
{
  return lhs.name() == rhs.name();
}

inline bool operator!=(const Rule &lhs, const Rule &rhs)
{
  return !(lhs == rhs);
}

inline std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
  return os << rule.name();
}
} // namespace fheco::trs
