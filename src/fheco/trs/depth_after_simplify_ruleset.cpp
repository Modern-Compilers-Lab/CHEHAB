#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include <memory>
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::depth_after_simplify_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};
  TermMatcher t{TermMatcherType::term, "t"};

  TermMatcher c0{TermMatcherType::const_, "c0"};
  TermMatcher c1{TermMatcherType::const_, "c1"};
  TermMatcher c2{TermMatcherType::const_, "c2"};
  TermMatcher c3{TermMatcherType::const_, "c3"};

  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};
  TermMatcher one{"one", PackedVal(func->slot_count(), 1)};
  TermMatcher m_one{"m_one", PackedVal(func->slot_count(), -1)};
  TermMatcher two{"two", PackedVal(func->slot_count(), 2)};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  auto cache = make_unique<TermsMetric>();

  vector<Rule> add_rules{
    {"assoc-balan-add-1", ((x + y) + z) + t, (x + y) + (z + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-2", (z + (x + y)) + t, (z + x) + (y + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-3", x + (y + (z + t)), (x + y) + (z + t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-4", x + ((z + t) + y), (x + z) + (t + y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},

    {"assoc-balan-add-sub-1", ((x + y) - z) + t, (x + y) - (z - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-2", (z - (x + y)) + t, (z - x) - (y - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-3", ((x - y) + z) + t, (x - y) + (z + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-4", (z + (x - y)) + t, (z + x) - (y - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-5", ((x - y) - z) + t, (x - y) - (z - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-6", (z - (x - y)) + t, (z - x) + (y + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-add-sub-7", x + (y - (z + t)), (x + y) - (z + t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-sub-8", x + ((z + t) - y), (x + z) + (t - y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-sub-9", x + (y + (z - t)), (x + y) + (z - t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-sub-10", x + ((z - t) + y), (x + z) - (t - y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-sub-11", x + (y - (z - t)), (x + y) - (z - t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-add-sub-12", x + ((z - t) - y), (x + z) - (t + y), Rule::has_less_ctxt_leaves(x, z, t, *cache)}};

  vector<Rule> sub_rules{
    {"assoc-balan-sub-1", ((x - y) - z) - t, (x - y) - (z + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-2", (z - (x - y)) - t, (z - x) + (y - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-3", x - (y - (z - t)), (x - y) - (z + t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-4", x - ((z - t) - y), (x - z) + (t + y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},

    {"assoc-balan-sub-add-1", ((x - y) + z) - t, (x - y) + (z - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-2", (z + (x - y)) - t, (z + x) - (y + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-3", ((x + y) - z) - t, (x + y) - (z + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-4", (z - (x + y)) - t, (z - x) - (y + t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-5", ((x + y) + z) - t, (x + y) + (z - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-6", (z + (x + y)) - t, (z + x) + (y - t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-sub-add-7", x - (y + (z - t)), (x - y) - (z - t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-add-8", x - ((z - t) + y), (x - z) + (t - y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-add-9", x - (y - (z + t)), (x - y) + (z + t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-add-10", x - ((z + t) - y), (x - z) - (t - y), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-add-11", x - (y + (z + t)), (x - y) - (z + t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-sub-add-12", x - ((z + t) + y), (x - z) - (t + y), Rule::has_less_ctxt_leaves(x, z, t, *cache)}};

  vector<Rule> rotate_rules{
    {"undo-nest-rotate-add-1", (x + (y << (n))) << m, (x << m) + (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-add-2", ((y << (n)) + x) << m, (y << (n + m) % func->slot_count()) + (x << m)},

    {"undo-nest-rotate-sub-1", (x - (y << (n))) << m, (x << m) - (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-sub-2", ((y << (n)) - x) << m, (y << (n + m) % func->slot_count()) - (x << m)},

    {"undo-nest-rotate-mul-1", (x * (y << (n))) << m, (x << m) * (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-mul-2", ((y << (n)) * x) << m, (y << (n + m) % func->slot_count()) * (x << m)}};

  vector<Rule> mul_rules{
    {"assoc-balan-mul-square-1", ((x * y) * x) * z, square(x) * (y * z), Rule::has_less_ctxt_leaves(z, x, y, *cache)},
    {"assoc-balan-mul-square-2", (x * (x * y)) * z, square(x) * (y * z), Rule::has_less_ctxt_leaves(z, x, y, *cache)},
    {"assoc-balan-mul-square-3", ((x * y) * y) * z, (x * z) * square(y), Rule::has_less_ctxt_leaves(z, x, y, *cache)},
    {"assoc-balan-mul-square-4", (y * (x * y)) * z, square(y) * (x * z), Rule::has_less_ctxt_leaves(z, x, y, *cache)},
    {"assoc-balan-mul-square-5", ((x * y) * z) * x, square(x) * (y * z)},
    {"assoc-balan-mul-square-6", x * ((x * y) * z), square(x) * (y * z)},
    {"assoc-balan-mul-square-7", ((x * y) * z) * y, (x * z) * square(y)},
    {"assoc-balan-mul-square-8", y * ((x * y) * z), square(y) * (x * z)},

    {"assoc-balan-mul-1", ((x * y) * z) * t, (x * y) * (z * t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-mul-2", (z * (x * y)) * t, (z * x) * (y * t), Rule::has_less_ctxt_leaves(t, x, y, *cache)},
    {"assoc-balan-mul-3", x * (y * (z * t)), (x * y) * (z * t), Rule::has_less_ctxt_leaves(x, z, t, *cache)},
    {"assoc-balan-mul-4", x * ((z * t) * y), (x * z) * (t * y), Rule::has_less_ctxt_leaves(x, z, t, *cache)}};

  return Ruleset{
    func,
    "depth_after_simplify_ruleset",
    {{ir::OpCode::Type::add, move(add_rules)},
     {ir::OpCode::Type::sub, move(sub_rules)},
     {ir::OpCode::Type::rotate, move(rotate_rules)},
     {ir::OpCode::Type::mul, move(mul_rules)}},
    move(cache)};
}
} // namespace fheco::trs
