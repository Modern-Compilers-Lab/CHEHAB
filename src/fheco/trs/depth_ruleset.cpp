#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include <memory>
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::depth_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher w{TermMatcherType::term, "w"};
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};
  TermMatcher t{TermMatcherType::term, "t"};

  TermMatcher c0{TermMatcherType::const_, "c0"};
  TermMatcher c1{TermMatcherType::const_, "c1"};

  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};
  TermMatcher one{"one", PackedVal(func->slot_count(), 1)};
  TermMatcher m_one{"m_one", PackedVal(func->slot_count(), -1)};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  auto cache = make_unique<TermsMetric>();

  vector<Rule> add_rules{
    {"add_0-1", x + zero, x},
    {"add_0-2", zero + x, x},

    {"simplify-add-negate-1-1", x + (-y), x - y},
    {"simplify-add-negate-1-2", (-y) + x, x - y},

    {"simplify-add-mul_negate-1", x * (-y) + z, z - x * y},
    {"simplify-add-mul_negate-2", (-y) * x + z, z - y * x},
    {"simplify-add-mul_negate-3", z + x * (-y), z - x * y},
    {"simplify-add-mul_negate-4", z + (-y) * x, z - y * x},

    {"part-fold-assoc-add-1", c0 + (c1 + x), (c0 + c1) + x},
    {"part-fold-assoc-add-2", c0 + (x + c1), (c0 + c1) + x},
    {"part-fold-assoc-add-3", (c0 + x) + c1, x + (c0 + c1)},
    {"part-fold-assoc-add-4", (x + c0) + c1, x + (c0 + c1)},

    {"part-fold-assoc-add-sub-1", c0 + (c1 - x), (c0 + c1) - x},
    {"part-fold-assoc-add-sub-2", c0 + (x - c1), (c0 - c1) + x},
    {"part-fold-assoc-add-sub-3", (c0 - x) + c1, (c0 + c1) - x},
    {"part-fold-assoc-add-sub-4", (x - c0) + c1, x + (c1 - c0)},

    {"simplify-add-sub-1-1", (x - y) + y, x},
    {"simplify-add-sub-1-2", x + (y - x), y},

    {"simplify-add-negate-2-1", x + (-y - z), x - (y + z)},
    {"simplify-add-negate-2-2", (-y - z) + x, x - (y + z)},

    {"simplify-add-sub-2-1", ((x - y) + z) + y, x + z},
    {"simplify-add-sub-2-2", (z + (x - y)) + y, z + x},
    {"simplify-add-sub-2-3", x + ((y - x) + z), y + z},
    {"simplify-add-sub-2-4", x + (z + (y - x)), z + y},

    {"simplify-add-sub-3-1", (x - y) + (y - z), x - z},
    {"simplify-add-sub-3-2", (x - y) + (z - x), z - y},
    {"simplify-add-sub-3-3", (x - y) + (y + z), x + z},
    {"simplify-add-sub-3-4", (x - y) + (z + y), x + z},

    {"simplify-add-sub-4-1", x + ((y - x) - z), y - z},
    {"simplify-add-sub-4-2", ((x - y) - z) + y, x - z},
    {"simplify-add-sub-4-3", x + (y - (x + z)), y - z},
    {"simplify-add-sub-4-4", x + (y - (z + x)), y - z},
    {"simplify-add-sub-4-5", (x - (y + z)) + y, x - z},
    {"simplify-add-sub-4-6", (x - (y + z)) + z, x - y},

    {"part-fold-zero_m-1", x + ((zero - y) - z), x - (y + z)},
    {"part-fold-zero_m-2", ((zero - x) - y) + z, z - (x + y)},

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
    {"sub_0", x - zero, x},

    {"sub_self", x - x, zero},

    {"simplify-sub-negate", x - (-y), x + y},

    {"simplify-sub-mul_negate-1", z - x * (-y), z + x * y},
    {"simplify-sub-mul_negate-2", z - (-y) * x, z + y * x},

    {"part-fold-assoc-sub-1-1", c0 - (c1 - x), (c0 - c1) + x},
    {"part-fold-assoc-sub-1-2", c0 - (x - c1), (c0 + c1) - x},
    {"part-fold-assoc-sub-1-3", (c0 - x) - c1, (c0 - c1) - x},
    {"part-fold-assoc-sub-1-4", (x - c0) - c1, x - (c1 - c0)},

    {"simplify-sub-1", (x + y) - x, y},
    {"simplify-sub-2", (x + y) - y, x},
    {"simplify-sub-3", x - (x + y), -y},
    {"simplify-sub-4", y - (x + y), -x},
    {"simplify-sub-5", (x - y) - x, -y},

    {"simplify-sub-2-1", (x + y) - (x + z), y - z},
    {"simplify-sub-2-2", (x + y) - (z + x), y - z},
    {"simplify-sub-2-3", (y + x) - (x + z), y - z},
    {"simplify-sub-2-4", (y + x) - (z + x), y - z},

    {"simplify-sub-3-1", ((x + y) + z) - x, y + z},
    {"simplify-sub-3-2", ((y + x) + z) - x, y + z},
    {"simplify-sub-3-3", (z + (x + y)) - x, z + y},
    {"simplify-sub-3-4", (z + (y + x)) - x, z + y},

    {"simplify-sub-4-1", x - (y + (x - z)), z - y},
    {"simplify-sub-4-2", x - ((x - y) + z), y - z},
    {"simplify-sub-4-3", (x + (y - z)) - y, x - z},
    {"simplify-sub-4-4", ((x - y) + z) - x, z - y},

    {"simplify-sub-5-1", x - (y + (x + z)), zero - (y + z)},
    {"simplify-sub-5-2", x - (y + (z + x)), zero - (y + z)},
    {"simplify-sub-5-3", x - ((x + y) + z), zero - (y + z)},
    {"simplify-sub-5-4", x - ((y + x) + z), zero - (y + z)},

    {"simplify-sub-6-1", (x + y) - (z + (w + x)), y - (z + w)},
    {"simplify-sub-6-2", (x + y) - (z + (w + y)), x - (z + w)},
    {"simplify-sub-6-3", (x + y) - (z + (x + w)), y - (z + w)},
    {"simplify-sub-6-4", (x + y) - ((x + z) + w), y - (z + w)},
    {"simplify-sub-6-5", (x + y) - ((x + z) + w), y - (z + w)},
    {"simplify-sub-6-6", (x + y) - ((y + z) + w), x - (z + w)},
    {"simplify-sub-6-7", (x + y) - ((z + x) + w), y - (z + w)},
    {"simplify-sub-6-8", (x + y) - ((z + y) + w), x - (z + w)},

    {"simplify-sub-6-1", (x - y) - (x + z), zero - y - z},
    {"simplify-sub-6-2", (x - y) - (z + x), zero - y - z},

    {"simplify-sub-7-1", ((x + y) - z) - x, y - z},
    {"simplify-sub-7-2", ((x + y) - z) - y, x - z},

    {"simplify-sub-8-1", zero - (x + (y - z)), z - (x + y)},
    {"simplify-sub-8-2", zero - ((x - y) + z), y - (x + z)},

    {"simplify-sub-9", ((x - y) - z) - x, zero - (y + z)},

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

  vector<Rule> negate_rules{
    {"fold-negate", -(-x), x}, {"fold-negate-add", -(x - y), y - x}, {"fold-negate-sub", -(-x - y), x + y}};

  vector<Rule> rotate_rules{
    {"rotate_0", x << 0, x},

    {"fold-rotate", x << n << m, x << ((m + n) % func->slot_count())},

    {"undo-nest-rotate-add-1", (x + (y << (n))) << m, (x << m) + (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-add-2", ((y << (n)) + x) << m, (y << (n + m) % func->slot_count()) + (x << m)},

    {"undo-nest-rotate-sub-1", (x - (y << (n))) << m, (x << m) - (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-sub-2", ((y << (n)) - x) << m, (y << (n + m) % func->slot_count()) - (x << m)},

    {"undo-nest-rotate-mul-1", (x * (y << (n))) << m, (x << m) * (y << (n + m) % func->slot_count())},
    {"undo-nest-rotate-mul-2", ((y << (n)) * x) << m, (y << (n + m) % func->slot_count()) * (x << m)}};

  vector<Rule> mul_rules{
    {"mul_0-1", x * zero, zero},
    {"mul_0-2", zero * x, zero},

    {"mul_1-1", x * one, x},
    {"mul_1-2", one * x, x},

    {"mul_m1-1", x * m_one, -x},
    {"mul_m1-2", m_one * x, -x},

    {"part-fold-assoc-mul-1", c0 * (c1 * x), (c0 * c1) * x},
    {"part-fold-assoc-mul-2", c0 * (x * c1), (c0 * c1) * x},
    {"part-fold-assoc-mul-3", (c0 * x) * c1, x * (c0 * c1)},
    {"part-fold-assoc-mul-4", (x * c0) * c1, x * (c0 * c1)},

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
    "depth_ruleset",
    {{ir::OpCode::Type::add, move(add_rules)},
     {ir::OpCode::Type::sub, move(sub_rules)},
     {ir::OpCode::Type::negate, move(negate_rules)},
     {ir::OpCode::Type::rotate, move(rotate_rules)},
     {ir::OpCode::Type::mul, move(mul_rules)}},
    move(cache)};
}
} // namespace fheco::trs
