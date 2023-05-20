#pragma once

#include "fheco/trs/rule.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/trs/subst.hpp"
#include <cstddef>
#include <cstdint>
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
  enum class RewriteHeuristic
  {
    bottom_up,
    top_down
  };

  TRS(std::shared_ptr<ir::Func> func, Ruleset ruleset) : func_{std::move(func)}, ruleset_{std::move(ruleset)} {}

  void run(RewriteHeuristic heuristic, bool global_analysis);

  void rewrite_term(std::size_t id, RewriteHeuristic heuristic, bool global_analysis);

private:
  bool match(
    const TermMatcher &term_matcher, ir::Term *term, Subst &subst, bool global_analysis, std::int64_t &rel_cost,
    ir::Term::PtrSet &to_delete) const;

  ir::Term *construct_term(
    const TermMatcher &term_matcher, const Subst &subst, const ir::Term::PtrSet &to_delete, bool global_analysis,
    std::int64_t &rel_cost, std::vector<std::size_t> &created_terms_ids);

  std::shared_ptr<ir::Func> func_;

  Ruleset ruleset_;
};
} // namespace fheco::trs
