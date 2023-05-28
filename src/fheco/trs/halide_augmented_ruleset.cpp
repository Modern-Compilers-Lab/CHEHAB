#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::halide_augmented_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher ct_x{TermMatcherType::cipher, "ct_x"};

  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher ct_y{TermMatcherType::cipher, "ct_y"};
  TermMatcher pt_y{TermMatcherType::plain, "pt_y"};

  TermMatcher z{TermMatcherType::term, "z"};
  TermMatcher ct_z{TermMatcherType::cipher, "ct_z"};

  TermMatcher c0{TermMatcherType::const_, "c0"};
  TermMatcher c1{TermMatcherType::const_, "c1"};
  TermMatcher c2{TermMatcherType::const_, "c2"};
  TermMatcher c3{TermMatcherType::const_, "c3"};

  TermMatcher zero{"zero", PackedVal(func->slot_count(), 0)};
  TermMatcher one{"one", PackedVal(func->slot_count(), 1)};
  TermMatcher m_one{"m_one", PackedVal(func->slot_count(), -1)};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  vector<Rule> add_rules{
    {"add_0-1", ct_x + zero, ct_x},
    {"add_0-2", zero + ct_x, ct_x},

    {"nest-rotate-add-1", (x << n) + (y << m), ((x << (n - m)) + y) << m,
     [n, m](const Subst subst) {
       auto n_steps = subst.get(n);
       auto m_steps = subst.get(m);
       return m_steps > 0 && n_steps > m_steps;
     }},
    {"nest-rotate-add-2", (x << n) + (y << m), (x + (y << (m - n))) << n,
     [n, m](const Subst subst) {
       auto n_steps = subst.get(n);
       auto m_steps = subst.get(m);
       return n_steps > 0 && m_steps > n_steps;
     }},

    {"simplify-add-mul_m1-1", x + y * m_one, x - y},
    {"simplify-add-mu1_m1-2", x * m_one + y, y - x},

    {"part-fold-assoc-add", (ct_x + c0) + c1, ct_x + (c0 + c1)},

    {"const-up-assoc-add-1", (x + c0) + y, (x + y) + c0},
    {"const-up-assoc-add-2", (c0 + x) + y, (x + y) + c0},
    {"const-up-assoc-add-3", x + (y + c0), (x + y) + c0},
    {"const-up-assoc-add-4", x + (c0 + y), (x + y) + c0},

    {"rotation-up-assoc-add-1", (x + (y << n)) + z, (x + z) + (y << n), Rule::is_not_rotation(z)},
    {"rotation-up-assoc-add-2", ((y << n) + x) + z, (x + z) + (y << n), Rule::is_not_rotation(z)},
    {"rotation-up-assoc-add-3", x + (y + (z << n)), (x + y) + (z << n), Rule::is_not_rotation(x)},
    {"rotation-up-assoc-add-4", x + ((z << n) + y), (x + y) + (z << n), Rule::is_not_rotation(x)},

    {"part-fold-assoc-add-sub", (c0 - ct_x) + c1, (c0 + c1) - ct_x},

    {"const-up-assoc-add-sub-1", (c0 - x) + y, (y - x) + c0},

    {"rotation-up-assoc-add-sub-1", ((x << n) - y) + z, (z - y) + (x << n), Rule::is_not_rotation(z)},

    {"simplify-add-sub-1-1", (x - y) + y, x},
    {"simplify-add-sub-1-2", x + (y - x), y},

    {"simplify-add-sub-2-1", ((x - y) + z) + y, x + z},
    {"simplify-add-sub-2-2", (z + (x - y)) + y, z + x},
    {"simplify-add-sub-2-3", x + ((y - x) + z), y + z},
    {"simplify-add-sub-2-4", x + (z + (y - x)), z + y},

    {"const-up-assoc-add-sub-2", x + (c0 - y), (x - y) + c0},

    {"rotation-up-assoc-add-sub-2", x + ((y << n) - z), (x - z) + (y << n), Rule::is_not_rotation(x)},

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
    {"fact_one-add-4", y * x + x, (y + one) * x, Rule::is_not_const(x, func)},

    {"merge-rotate-add", (x << n) + (y << n), (x + y) << n}};

  vector<Rule> rotate_rules{
    {"rotate_0", x << 0, x},

    {"part-fold-rotate-add-1", (x + c0) << n, (x << n) + (c0 << n)},
    {"part-fold-rotate-add-2", (c0 + x) << n, (c0 << n) + (x << n)},

    {"part-fold-rotate-sub-1", (x - c0) << n, (x << n) - (c0 << n)},
    {"part-fold-rotate-sub-2", (c0 - x) << n, (c0 << n) - (x << n)},

    {"part-fold-rotate-mul-1", (x * c0) << n, (x << n) * (c0 << n)},
    {"part-fold-rotate-mul-2", (c0 * x) << n, (c0 << n) * (x << n)},

    {"fold-rotate", x << n << m, x << ((m + n) % func->slot_count())},

    {"nest-large-steps-rotate-add-1-1", (x + (y << n)) << m, (x << m) + (y << (n + m) % func->slot_count()),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }},
    {"nest-large-steps-rotate-add-1-2", ((y << n) + x) << m, (y << (n + m) % func->slot_count()) + (x << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }},

    {"nest-large-steps-rotate-sub-1-1", (x - (y << n)) << m, (x << m) - (y << (n + m) % func->slot_count()),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }},
    {"nest-large-steps-rotate-sub-1-2", ((y << n) - x) << m, (y << (n + m) % func->slot_count()) - (x << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }},

    {"nest-large-steps-rotate-mul-1-1", (x * (y << n)) << m, (x << m) * (y << (n + m) % func->slot_count()),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }},
    {"nest-large-steps-rotate-mul-1-2", ((y << n) * x) << m, (y << (n + m) % func->slot_count()) * (x << m),
     [n, m, slot_count = func->slot_count()](const Subst subst) {
       return subst.get(n) + subst.get(m) >= slot_count;
     }}};

  vector<Rule> mul_rules{
    {"mul_0-1", zero * ct_x, zero},
    {"mul_0-2", ct_x * zero, zero},
    {"mul_1-1", one * ct_x, ct_x},
    {"mul_1-2", ct_x * one, ct_x},

    {"mul-to-square", x * x, square(x)},

    {"nest-rotate-mul-1", (x << n) * (y << m), ((x << (n - m)) * y) << m,
     [n, m](const Subst subst) {
       auto n_steps = subst.get(n);
       auto m_steps = subst.get(m);
       return m_steps > 0 && n_steps > m_steps;
     }},
    {"nest-rotate-mul-2", (x << n) * (y << m), (x * (y << (m - n))) << n,
     [n, m](const Subst subst) {
       auto n_steps = subst.get(n);
       auto m_steps = subst.get(m);
       return n_steps > 0 && m_steps > n_steps;
     }},

    {"part-fold-dist-add-1-1", (ct_x + c0) * (ct_x + c1), ct_x * (ct_x + (c0 + c1)) + (c0 * c1)},
    {"part-fold-dist-add-1-2", (ct_x * c0 + c1) * (ct_x + c2), ct_x * (ct_x * c0 + (c1 + c0 * c2)) + (c1 * c2)},
    {"part-fold-dist-add-1-3", (ct_x + c2) * (ct_x * c0 + c1), ct_x * (ct_x * c0 + (c1 + c0 * c2)) + (c1 * c2)},
    {"part-fold-dist-add-1-4", (ct_x * c0 + c1) * (ct_x * c2 + c3),
     ct_x * (ct_x * (c0 * c2) + (c0 * c3 + c1 * c2)) + (c1 * c3)},

    {"part-fold-dist-add-2", (ct_x + c0) * c1, ct_x * c1 + (c0 * c1)},
    {"part-fold-dist-sub", (c0 - ct_x) * c1, ct_x * (-c1) + (c0 * c1)},

    {"part-fold-dist-zero_m-1", (zero - x) * y, zero - x * y},
    {"part-fold-dist-zero_m-2", x * (zero - y), zero - x * y},

    {"part-fold-assoc-mul-1", (x * c0) * c1, x * (c0 * c1)},
    {"part-fold-assoc-mul-2", (c0 * x) * c1, x * (c0 * c1)},
    {"part-fold-assoc-mul-3", c1 * (x * c0), (c1 * c0) * x},
    {"part-fold-assoc-mul-4", c1 * (c0 * x), (c1 * c0) * x},

    {"const-up-assoc-mul-1", (x * c0) * y, (x * y) * c0, Rule::is_not_const(y, func)},
    {"const-up-assoc-mul-2", x * (y * c0), (x * y) * c0},

    {"rotation-up-assoc-mul-1", (x * (y << n)) * z, (x * z) * (y << n), Rule::is_not_rotation(z)},
    {"rotation-up-assoc-mul-2", x * (y * (z << n)), (x * y) * (z << n), Rule::is_not_rotation(x)}};

  return Ruleset{move(func), move(add_rules), {}, {}, move(rotate_rules), {}, move(mul_rules)};
}
} // namespace fheco::trs
