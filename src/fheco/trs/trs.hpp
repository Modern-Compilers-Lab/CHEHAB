#pragma once

#include "fheco/trs/common.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/trs/substitution.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace fheco::ir
{
class Func;

class Term;
} // namespace fheco::ir

namespace fheco::trs
{
class TRS
{
public:
  TRS(std::shared_ptr<ir::Func> func) : func_{func}, ruleset_{std::move(func), "void", Ruleset::RulesByRootOp{}} {}

  TRS(Ruleset ruleset) : func_{ruleset.func()}, ruleset_{std::move(ruleset)} {}

  bool run(
    RewriteHeuristic heuristic, std::int64_t max_iter = std::numeric_limits<std::int64_t>::max(),
    bool rewrite_created_sub_terms = true, bool global_analysis = false);

  bool apply_rule(ir::Term *term, const Rule &rule);

  bool rewrite_term(
    std::size_t id, RewriteHeuristic heuristic, std::int64_t &max_iter, bool rewrite_created_sub_terms,
    bool global_analysis);

  inline const Ruleset &ruleset() const { return ruleset_; }

private:
  bool match(
    const TermMatcher &term_matcher, ir::Term *term, Substitution &subst, bool global_analysis, double &rel_cost,
    ir::Term::PtrSet &to_delete) const;

  ir::Term *construct_term(
    const TermMatcher &term_matcher, const Substitution &subst, const ir::Term::PtrSet &to_delete, bool global_analysis,
    double &rel_cost, std::vector<std::size_t> &created_terms_ids);

  std::shared_ptr<ir::Func> func_;

  Ruleset ruleset_;
};
} // namespace fheco::trs
