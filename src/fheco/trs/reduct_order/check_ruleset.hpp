#pragma once

#include "fheco/trs/reduct_order/common.hpp"
#include "fheco/trs/ruleset.hpp"
#include <functional>
#include <string_view>
#include <utility>
#include <vector>

namespace fheco::trs
{
using ComponOrder = std::function<CompResult(const TermMatcher &, const TermMatcher &)>;

using LexicoProductOrder = std::vector<std::pair<std::string_view, ComponOrder>>;

extern const LexicoProductOrder depth_order;

extern const LexicoProductOrder ops_cost_order;

extern const LexicoProductOrder joined_order;

void check_ruleset(const Ruleset &ruleset, const LexicoProductOrder &reduct_order, bool throw_on_failure = true);

void check_rule(const Rule &rule, const LexicoProductOrder &reduct_order, bool throw_on_failure);
} // namespace fheco::trs
