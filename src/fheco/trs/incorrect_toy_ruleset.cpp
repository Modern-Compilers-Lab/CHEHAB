#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::incorrect_toy_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};

  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};

  vector<Rule> add_rules{
    {"add_0-2", zero + x, x}, {"simplify-add-sub-1-1", (x - y) + y, x}, {"fact-add", x * y + x * z, x * (y + x)}};

  return Ruleset{func, "incorrect_toy_ruleset", {{ir::OpCode::Type::add, move(add_rules)}}};
}
} // namespace fheco::trs
