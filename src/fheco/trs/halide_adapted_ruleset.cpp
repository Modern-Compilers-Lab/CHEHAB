#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::halide_adapted_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher u{TermMatcherType::term, "u"};

  TermMatcher w{TermMatcherType::term, "w"};

  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher ct_x{TermMatcherType::cipher, "ct_x"};

  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher ct_y{TermMatcherType::cipher, "ct_y"};
  TermMatcher pt_y{TermMatcherType::plain, "pt_y"};

  TermMatcher z{TermMatcherType::term, "z"};
  TermMatcher ct_z{TermMatcherType::cipher, "ct_z"};

  TermMatcher t{TermMatcherType::term, "t"};

  TermMatcher c0{TermMatcherType::const_, "c0"};
  TermMatcher c1{TermMatcherType::const_, "c1"};
  TermMatcher c2{TermMatcherType::const_, "c2"};
  TermMatcher c3{TermMatcherType::const_, "c3"};
  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};
  TermMatcher one{"one", PackedVal(func->slot_count(), 1)};
  TermMatcher m_one{"m_one", PackedVal(func->slot_count(), -1)};

  vector<Rule> add_rules{
    {"add_0-1", x + zero, x},
    {"add_0-2", zero + x, x},

    {"simplify-add-mul_m1-1", x + y * m_one, x - y},
    {"simplify-add-mu1_m1-2", x * m_one + y, y - x},

    {"part-fold-assoc-add", (x + c0) + c1, x + (c0 + c1)},

    {"const-up-assoc-add-1", (x + c0) + y, (x + y) + c0},
    {"const-up-assoc-add-2", x + (y + c0), (x + y) + c0},

    {"part-fold-assoc-add-sub", (c0 - x) + c1, (c0 + c1) - x},

    {"const-up-assoc-add-sub-1", (c0 - x) + y, (y - x) + c0},

    {"simplify-add-sub-1-1", (x - y) + y, x},
    {"simplify-add-sub-1-2", x + (y - x), y},

    {"simplify-add-sub-2-1", ((x - y) + z) + y, x + z},
    {"simplify-add-sub-2-2", (z + (x - y)) + y, z + x},
    {"simplify-add-sub-2-3", x + ((y - x) + z), y + z},
    {"simplify-add-sub-2-4", x + (z + (y - x)), z + y},

    {"const-up-assoc-add-sub-2", x + (c0 - y), (x - y) + c0},

    {"simplify-add-sub-3-1", (x - y) + (y - z), x - z},
    {"simplify-add-sub-3-2", (x - y) + (z - x), z - y},
    {"simplify-add-sub-3-3", (x - y) + (y + z), x + z},
    {"simplify-add-sub-3-4", (x - y) + (z + y), x + z},

    {"simplify-add-sub-4-1", x + ((y - x) - z), y - z},
    {"simplify-add-sub-4-1", ((x - y) - z) + y, x - z},
    {"simplify-add-sub-4-1", x + (y - (x + z)), y - z},
    {"simplify-add-sub-4-1", x + (y - (z + x)), y - z},
    {"simplify-add-sub-4-1", (x - (y + z)) + y, x - z},
    {"simplify-add-sub-4-1", (x - (y + z)) + z, x - y},

    {"part-fold-zero_m-1", x + ((zero - y) - z), x - (y + z)},
    {"part-fold-zero_m-2", ((zero - x) - y) + z, z - (x + y)},

    {"part-fold-assoc-add-sub-3", ((c0 - x) - y) + c1, ((c0 + c1) - y) - x},

    {"fact-add-1-1", x * ct_y + z * ct_y, (x + z) * ct_y},
    {"fact-add-1-2", ct_x * pt_y + ct_z * pt_y, (ct_x + ct_z) * pt_y},
    {"fact-add-2-1", x * ct_y + ct_y * z, (x + z) * ct_y},
    {"fact-add-2-2", ct_x * pt_y + pt_y * ct_z, (ct_x + ct_z) * pt_y},
    {"fact-add-3-1", ct_y * x + z * ct_y, ct_y * (x + z)},
    {"fact-add-3-2", pt_y * ct_x + ct_z * pt_y, pt_y * (ct_x + ct_z)},
    {"fact-add-4-1", ct_y * x + ct_y * z, ct_y * (x + z)},
    {"fact-add-4-2", pt_y * ct_x + pt_y * ct_z, pt_y * (ct_x + ct_z)},

    {"fact_one-add-1", x + x * y, x * (y + one)},
    {"fact_one-add-2", x + y * x, (y + one) * x},
    {"fact_one-add-3", x * y + x, x * (y + one)},
    {"fact_one-add-4", y * x + x, (y + one) * x, Rule::is_not_const(x, func)}};

  vector<Rule> sub_rules{
    {"sub_0", x - zero, x},

    {"sub_self", x - x, zero},

    {"simplify-sub-1", (x + y) - x, y},
    {"simplify-sub-2", (x + y) - y, x},
    {"simplify-sub-3", x - (x + y), -y},
    {"simplify-sub-4", y - (x + y), -x},
    {"simplify-sub-5", (x - y) - x, -y},

    {"part-fold-assoc-sub-1-1", (x + c0) - c1, x + (c0 - c1)},
    {"part-fold-assoc-sub-1-2", (x + c0) - (c1 - y), (x + y) + (c0 - c1)},
    {"part-fold-assoc-sub-1-3", (x + c0) - (y + c1), (x - y) + (c0 - c1)},

    {"const-up-assoc-sub-1", (x + c0) - y, (x - y) + c0},

    {"part-fold-assoc-sub-4", (c0 - x) - (c1 - y), (y - x) + (c0 - c1)},
    {"part-fold-assoc-sub-5", (c0 - x) - (y + c1), (c0 - c1) - (x + y)},

    {"part-fold-assoc-sub-negate", x - (y - z), x + (z - y)},

    {"pconst-up-assoc-sub-2", x - (y + c0), (x - y) - c0},

    {"part-fold-assoc-sub-5", (c0 - x) - c1, (c0 - c1) - x},

    {"fact-sub-1-1-1", x * ct_y - z * ct_y, (x - z) * ct_y},
    {"fact-sub-1-1-2", ct_x * pt_y - ct_z * pt_y, (ct_x - ct_z) * pt_y},
    {"fact-sub-1-2-1", x * ct_y - ct_y * z, (x - z) * ct_y},
    {"fact-sub-1-2-2", ct_x * pt_y - pt_y * ct_z, (ct_x - ct_z) * pt_y},
    {"fact-sub-1-3-1", ct_y * x - z * ct_y, ct_y * (x - z)},
    {"fact-sub-1-3-2", pt_y * ct_x - ct_z * pt_y, pt_y * (ct_x - ct_z)},
    {"fact-sub-1-4-1", ct_y * x - ct_y * z, ct_y * (x - z)},
    {"fact-sub-1-4-2", pt_y * ct_x - pt_y * ct_z, pt_y * (ct_x - ct_z)},

    {"fact-sub-2-1-1", (u + x * ct_y) - z * ct_y, u + (x - z) * ct_y},
    {"fact-sub-2-1-2", (u + ct_x * pt_y) - ct_z * pt_y, u + (ct_x - ct_z) * pt_y},
    {"fact-sub-2-2-1", (u + x * ct_y) - ct_y * z, u + (x - z) * ct_y},
    {"fact-sub-2-2-2", (u + ct_x * pt_y) - pt_y * ct_z, u + (ct_x - ct_z) * pt_y},
    {"fact-sub-2-3-1", (u + ct_y * x) - z * ct_y, u + ct_y * (x - z)},
    {"fact-sub-2-3-2", (u + pt_y * ct_x) - ct_z * pt_y, u + pt_y * (ct_x - ct_z)},
    {"fact-sub-2-4-1", (u + ct_y * x) - ct_y * z, u + ct_y * (x - z)},
    {"fact-sub-2-4-2", (u + pt_y * ct_x) - pt_y * ct_z, u + pt_y * (ct_x - ct_z)},
    {"fact-sub-2-5-1", (u - x * ct_y) - z * ct_y, u - (x + z) * ct_y},
    {"fact-sub-2-5-2", (u - ct_x * pt_y) - ct_z * pt_y, u - (ct_x + ct_z) * pt_y},
    {"fact-sub-2-6-1", (u - x * ct_y) - ct_y * z, u - (x + z) * ct_y},
    {"fact-sub-2-6-2", (u - ct_x * pt_y) - pt_y * ct_z, u - (ct_x + ct_z) * pt_y},
    {"fact-sub-2-7-1", (u - ct_y * x) - z * ct_y, u - ct_y * (x + z)},
    {"fact-sub-2-7-2", (u - pt_y * ct_x) - ct_z * pt_y, u - pt_y * (ct_x + ct_z)},
    {"fact-sub-2-8-1", (u - ct_y * x) - ct_y * z, u - ct_y * (x + z)},
    {"fact-sub-2-8-2", (u - pt_y * ct_x) - pt_y * ct_z, u - pt_y * (ct_x + ct_z)},
    {"fact-sub-2-9-1", (x * ct_y + u) - z * ct_y, u + (x - z) * ct_y},
    {"fact-sub-2-9-2", (ct_x * pt_y + u) - ct_z * pt_y, u + (ct_x - ct_z) * pt_y},
    {"fact-sub-2-10-1", (x * ct_y + u) - ct_y * z, u + (x - z) * ct_y},
    {"fact-sub-2-10-2", (ct_x * pt_y + u) - pt_y * ct_z, u + (ct_x - ct_z) * pt_y},
    {"fact-sub-2-11-1", (ct_y * x + u) - z * ct_y, u + ct_y * (x - z)},
    {"fact-sub-2-11-2", (pt_y * ct_x + u) - ct_z * pt_y, u + pt_y * (ct_x - ct_z)},
    {"fact-sub-2-12-1", (ct_y * x + u) - ct_y * z, u + ct_y * (x - z)},
    {"fact-sub-2-12-2", (pt_y * ct_x + u) - pt_y * ct_z, u + pt_y * (ct_x - ct_z)},
    {"fact-sub-2-13-1", (x * ct_y - u) - z * ct_y, (x - z) * ct_y - u},
    {"fact-sub-2-13-2", (ct_x * pt_y - u) - ct_z * pt_y, (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-14-1", (x * ct_y - u) - ct_y * z, (x - z) * ct_y - u},
    {"fact-sub-2-14-2", (ct_x * pt_y - u) - pt_y * ct_z, (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-15-1", (ct_y * x - u) - z * ct_y, ct_y * (x - z) - u},
    {"fact-sub-2-15-2", (pt_y * ct_x - u) - ct_z * pt_y, pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-16-1", (ct_y * x - u) - ct_y * z, ct_y * (x - z) - u},
    {"fact-sub-2-16-2", (pt_y * ct_x - u) - pt_y * ct_z, pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-17-1", x * ct_y - (u + z * ct_y), (x - z) * ct_y - u},
    {"fact-sub-2-17-2", ct_x * pt_y - (u + ct_z * pt_y), (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-18-1", x * ct_y - (u + ct_y * z), (x - z) * ct_y - u},
    {"fact-sub-2-18-2", ct_x * pt_y - (u + pt_y * ct_z), (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-19-1", ct_y * x - (u + z * ct_y), ct_y * (x - z) - u},
    {"fact-sub-2-19-2", pt_y * ct_x - (u + ct_z * pt_y), pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-20-1", ct_y * x - (u + ct_y * z), ct_y * (x - z) - u},
    {"fact-sub-2-20-2", pt_y * ct_x - (u + pt_y * ct_z), pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-21-1", x * ct_y - (u - z * ct_y), (x + z) * ct_y - u},
    {"fact-sub-2-21-2", ct_x * pt_y - (u - ct_z * pt_y), (ct_x + ct_z) * pt_y - u},
    {"fact-sub-2-22-1", x * ct_y - (u - ct_y * z), (x + z) * ct_y - u},
    {"fact-sub-2-22-2", ct_x * pt_y - (u - pt_y * ct_z), (ct_x + ct_z) * pt_y - u},
    {"fact-sub-2-23-1", ct_y * x - (u - z * ct_y), ct_y * (x + z) - u},
    {"fact-sub-2-23-2", pt_y * ct_x - (u - ct_z * pt_y), pt_y * (ct_x + ct_z) - u},
    {"fact-sub-2-24-1", ct_y * x - (u - ct_y * z), ct_y * (x + z) - u},
    {"fact-sub-2-24-2", pt_y * ct_x - (u - pt_y * ct_z), pt_y * (ct_x + ct_z) - u},
    {"fact-sub-2-25-1", x * ct_y - (z * ct_y + u), (x - z) * ct_y - u},
    {"fact-sub-2-25-2", ct_x * pt_y - (ct_z * pt_y + u), (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-26-1", x * ct_y - (ct_y * z + u), (x - z) * ct_y - u},
    {"fact-sub-2-26-2", ct_x * pt_y - (pt_y * ct_z + u), (ct_x - ct_z) * pt_y - u},
    {"fact-sub-2-27-1", ct_y * x - (z * ct_y + u), ct_y * (x - z) - u},
    {"fact-sub-2-27-2", pt_y * ct_x - (ct_z * pt_y + u), pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-28-1", ct_y * x - (ct_y * z + u), ct_y * (x - z) - u},
    {"fact-sub-2-28-2", pt_y * ct_x - (pt_y * ct_z + u), pt_y * (ct_x - ct_z) - u},
    {"fact-sub-2-29-1", x * ct_y - (z * ct_y - u), (x - z) * ct_y + u},
    {"fact-sub-2-29-2", ct_x * pt_y - (ct_z * pt_y - u), (ct_x - ct_z) * pt_y + u},
    {"fact-sub-2-30-1", x * ct_y - (ct_y * z - u), (x - z) * ct_y + u},
    {"fact-sub-2-30-2", ct_x * pt_y - (pt_y * ct_z - u), (ct_x - ct_z) * pt_y + u},
    {"fact-sub-2-31-1", ct_y * x - (z * ct_y - u), ct_y * (x - z) + u},
    {"fact-sub-2-31-2", pt_y * ct_x - (ct_z * pt_y - u), pt_y * (ct_x - ct_z) + u},
    {"fact-sub-2-32-1", ct_y * x - (ct_y * z - u), ct_y * (x - z) + u},
    {"fact-sub-2-32-2", pt_y * ct_x - (pt_y * ct_z - u), pt_y * (ct_x - ct_z) + u},

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

    {"fact_one-sub-1", x * y - x, x * (y - one)},
    {"fact_one-sub-2", x * y - y, (x - one) * y},
    {"fact_one-sub-3", x - x * y, x * (one - y)},
    {"fact_one-sub-4", x - y * x, (one - y) * x}};

  vector<Rule> mul_rules{
    {"mul_0-1", zero * x, zero},
    {"mul_0-2", x * zero, zero},
    {"mul_1-1", one * x, x},
    {"mul_1-2", x * one, x},

    {"part-fold-dist-add-1-1", (ct_x + c0) * (ct_x + c1), ct_x * (ct_x + (c0 + c1)) + (c0 * c1)},
    {"part-fold-dist-add-1-2", (ct_x * c0 + c1) * (ct_x + c2), ct_x * (ct_x * c0 + (c1 + c0 * c2)) + (c1 * c2)},
    {"part-fold-dist-add-1-3", (ct_x + c2) * (ct_x * c0 + c1), ct_x * (ct_x * c0 + (c1 + c0 * c2)) + (c1 * c2)},
    {"part-fold-dist-add-1-4", (ct_x * c0 + c1) * (ct_x * c2 + c3),
     ct_x * (ct_x * (c0 * c2) + (c0 * c3 + c1 * c2)) + (c1 * c3)},

    {"part-fold-dist-add-2", (ct_x + c0) * c1, ct_x * c1 + (c0 * c1)},
    {"part-fold-dist-sub", (c0 - ct_x) * c1, ct_x * (-c1) + (c0 * c1)},

    {"part-fold-dist-zero_m-1", (zero - x) * y, zero - x * y},
    {"part-fold-dist-zero_m-2", x * (zero - y), zero - x * y},

    {"part-fold-assoc-mul", (x * c0) * c1, x * (c0 * c1)},

    {"const-up-assoc-mul-1", (x * c0) * y, (x * y) * c0, Rule::is_not_const(y, func)},
    {"const-up-assoc-mul-2", x * (y * c0), (x * y) * c0}};

  return Ruleset{
    move(func),
    "halide_adapted_ruleset",
    {{ir::OpCode::Type::add, move(add_rules)},
     {ir::OpCode::Type::sub, move(sub_rules)},
     {ir::OpCode::Type::mul, move(mul_rules)}}};
}
} // namespace fheco::trs
