#include "fheco/trs/ruleset.hpp"
#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/term_matcher.hpp"

using namespace std;

namespace fheco::trs
{
Ruleset::Ruleset(const std::shared_ptr<ir::Func> &func)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};

  TermMatcher const1{TermMatcherType::const_, "const1"};
  TermMatcher const2{TermMatcherType::const_, "const2"};
  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};
  TermMatcher one{"one", PackedVal(func->slot_count(), 1)};

  TermMatcher c_x{TermMatcherType::cipher, "ctxt_x"};
  TermMatcher c_y{TermMatcherType::cipher, "ctxt_y"};
  TermMatcher c_z{TermMatcherType::cipher, "ctxt_z"};

  TermMatcher p_x{TermMatcherType::plain, "ptxt_x"};
  TermMatcher p_y{TermMatcherType::plain, "ptxt_y"};
  TermMatcher p_z{TermMatcherType::plain, "ptxt_z"};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  add_rules_ = {
    {"add0-1", x + zero, x},
    {"add0-2", zero + x, x},
    {"part-fold-assoc-add-1", const1 + (const2 + c_x), (const1 + const2) + c_x},
    {"part-fold-assoc-add-2", const1 + (c_x + const2), (const1 + const2) + c_x},
    {"part-fold-assoc-add-3", (const1 + c_x) + const2, c_x + (const1 + const2)},
    {"part-fold-assoc-add-4", (c_x + const1) + const2, c_x + (const1 + const2)},
    {"part-fold-assoc-add-sub-1", const1 + (const2 - c_x), (const1 + const2) - c_x},
    {"part-fold-assoc-add-sub-2", const1 + (c_x - const2), (const1 - const2) + c_x},
    {"part-fold-assoc-add-sub-3", (const1 - c_x) + const2, -c_x + (const1 + const2)},
    {"part-fold-assoc-add-sub-4", (c_x - const1) + const2, c_x + (-const1 + const2)},
    {"merge-rotate-add", (x << n) + (y << n), (x + y) << n},
    {"nest-rotate-add-1", (x << n) + (y << m), (x + (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-add-2", (x << n) + (y << m), ((x << (n - m)) + y) << m,
     [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }},
    {"fact-add-1-1", x * c_y + z * c_y, (x + z) * c_y},
    {"fact-add-1-2", c_x * p_y + c_z * p_y, (c_x + c_z) * p_y},
    {"fact-add-2-1", x * c_y + c_y * z, (x + z) * c_y},
    {"fact-add-2-2", c_x * p_y + p_y * c_z, (c_x + c_z) * p_y},
    {"fact-add-3-1", c_y * x + z * c_y, c_y * (x + z)},
    {"fact-add-3-2", p_y * c_x + c_z * p_y, p_y * (c_x + c_z)},
    {"fact-add-4-1", c_y * x + c_y * z, c_y * (x + z)},
    {"fact-add-4-2", p_y * c_x + p_y * c_z, p_y * (c_x + c_z)}};

  sub_rules_ = {
    {"sub0-1", x - zero, x},
    {"sub0-2", zero - x, -x},
    {"part-fold-assoc-sub-1", const1 - (const2 - c_x), (const1 - const2) - c_x},
    {"part-fold-assoc-sub-2", const1 - (c_x - const2), (const1 - const2) - c_x},
    {"part-fold-assoc-sub-3", (const1 - c_x) - const2, c_x - (const1 - const2)},
    {"part-fold-assoc-sub-4", (c_x - const1) - const2, c_x - (const1 - const2)},
    {"part-fold-assoc-sub-add-1", const1 - (const2 + c_x), (const1 - const2) - c_x},
    {"part-fold-assoc-sub-add-2", const1 - (c_x + const2), (const1 - const2) + c_x},
    {"part-fold-assoc-sub-add-3", (const1 + c_x) - const2, c_x + (const1 - const2)},
    {"part-fold-assoc-sub-add-4", (c_x + const1) - const2, c_x + (const1 - const2)},
    {"merge-rotate-sub", (x << n) - (y << n), (x - y) << n},
    {"nest-rotate-sub-1", (x << n) - (y << m), (x - (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-sub-2", (x << n) - (y << m), ((x << (n - m)) - y) << m,
     [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }},
    {"fact-sub-1-1", x * c_y - z * c_y, (x - z) * c_y},
    {"fact-sub-1-2", c_x * p_y - c_z * p_y, (c_x - c_z) * p_y},
    {"fact-sub-2-1", x * c_y - c_y * z, (x - z) * c_y},
    {"fact-sub-2-2", c_x * p_y - p_y * c_z, (c_x - c_z) * p_y},
    {"fact-sub-3-1", c_y * x - z * c_y, c_y * (x - z)},
    {"fact-sub-3-2", p_y * c_x - c_z * p_y, p_y * (c_x - c_z)},
    {"fact-sub-4-1", c_y * x - c_y * z, c_y * (x - z)},
    {"fact-sub-4-2", p_y * c_x - p_y * c_z, p_y * (c_x - c_z)}};

  rotate_rules_ = {
    {"rotate0", x << 0, x},
    {"fold-rotate", x << n << m, x << ((m + n) % func->slot_count())},
    {"part-fold-rotate-add-1", (x + const1) << n, (x << n) + (const1 << n)},
    {"part-fold-rotate-add-2", (const1 + x) << n, (const1 << n) + (x << n)},
    {"part-fold-rotate-sub-1", (x - const1) << n, (x << n) - (const1 << n)},
    {"part-fold-rotate-sub-2", (const1 - x) << n, (const1 << n) - (x << n)},
    {"part-fold-rotate-mul-1", (x * const1) << n, (x << n) * (const1 << n)},
    {"part-fold-rotate-mul-2", (const1 * x) << n, (const1 << n) * (x << n)},
    {"rev-nest-rotate-add-1", (x + (y << n)) << m, (x << m) + y,
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-add-2", ((x << n) + y) << m, x + (y << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-sub-1", (x - (y << n)) << m, (x << m) - y,
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-sub-2", ((x << n) - y) << m, x - (y << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-mul-1", (x * (y << n)) << m, (x << m) * y,
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-mul-2", ((x << n) * y) << m, x * (y << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
  };

  mul_rules_ = {
    {"mul0-1", x * zero, zero},
    {"mul0-2", zero * x, zero},
    {"mul1-1", x * one, x},
    {"mul1-2", one * x, x},
    {"part-fold-dist-add-1", const1 * (const2 + c_x), const1 * const2 + const1 * c_x},
    {"part-fold-dist-add-2", const1 * (c_x + const2), const1 * c_x + const1 * const2},
    {"part-fold-dist-add-3", (const1 + c_x) * const2, const1 * const2 + c_x * const2},
    {"part-fold-dist-add-4", (c_x + const1) * const2, c_x * const2 + const1 * const2},
    {"part-fold-dist-sub-1", const1 * (const2 - c_x), const1 * const2 - const1 * c_x},
    {"part-fold-dist-sub-2", const1 * (c_x - const2), const1 * c_x - const1 * const2},
    {"part-fold-dist-sub-3", (const1 - c_x) * const2, const1 * const2 - c_x * const2},
    {"part-fold-dist-sub-4", (c_x - const1) * const2, c_x * const2 - const1 * const2},
    {"part-fold-dist-sub-4", (c_x - const1) * const2, c_x * const2 - const1 * const2},
    {"part-fold-assoc-mul-1", const1 * (const2 * c_x), (const1 * const2) * c_x},
    {"part-fold-assoc-mul-2", const1 * (c_x * const2), (const1 * const2) * c_x},
    {"part-fold-assoc-mul-3", (const1 * c_x) * const2, c_x * (const1 * const2)},
    {"part-fold-assoc-mul-4", (c_x * const1) * const2, c_x * (const1 * const2)},
    {"merge-rotate-mul", (x << n) * (y << n), (x * y) << n},
    {"nest-rotate-mul-1", (x << n) * (y << m), (x * (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-mul-2", (x << n) * (y << m), ((x << (n - m)) * y) << m, [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }}};

  misc_rules_ = {};
}

const vector<Rule> &Ruleset::pick_rules(const ir::OpCode &op_code) const
{
  switch (op_code.type())
  {
  case ir::OpCode::Type::nop:
    throw invalid_argument("cannot pick rules for nop");

  case ir::OpCode::Type::add:
    return add_rules();

  case ir::OpCode::Type::sub:
    return sub_rules();

  case ir::OpCode::Type::rotate:
    return rotate_rules();

  case ir::OpCode::Type::mul:
    return mul_rules();

  default:
    return misc_rules();
  }
}
} // namespace fheco::trs
