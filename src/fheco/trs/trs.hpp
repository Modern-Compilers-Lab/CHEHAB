#pragma once

#include "fheco/trs/rule.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/trs/subst.hpp"
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
  TRS(std::shared_ptr<ir::Func> func) : func_{std::move(func)}, ruleset_{func_} {}

  void run();

  void rewrite_term(ir::Term *term);

private:
  bool match(const TermMatcher &term_matcher, ir::Term *term, Subst &subst, std::int64_t &rel_cost) const;

  ir::Term *construct_term(
    const TermMatcher &term_matcher, const Subst &subst, std::int64_t &rel_cost,
    std::vector<ir::Term *> &created_terms);

  std::shared_ptr<ir::Func> func_;

  Ruleset ruleset_;
};
} // namespace fheco::trs
