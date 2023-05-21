#include "fheco/dsl/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/util/common.hpp"
#include <cstdint>
#include <utility>

using namespace std;

namespace fheco::trs
{

Ruleset Ruleset::depth_opt_ruleset(size_t slot_count)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};
  TermMatcher t{TermMatcherType::term, "t"};

  auto assoc_balan_cond = [](const TermMatcher &x, const TermMatcher &y, const TermMatcher &t) {
    return [x, y, t](const Subst &subst) {
      TermsMetric dp;
      auto x_term = subst.get(x);
      auto y_term = subst.get(y);
      auto t_term = subst.get(t);

      int64_t x_ctxt_leaves_count = 0;
      if (x_term->type() == ir::Term::Type::cipher)
      {
        count_ctxt_leaves(x_term, dp);
        x_ctxt_leaves_count = dp.at(x_term);
      }

      int64_t y_ctxt_leaves_count = 0;
      if (y_term->type() == ir::Term::Type::cipher)
      {
        count_ctxt_leaves(y_term, dp);
        y_ctxt_leaves_count = dp.at(y_term);
      }

      int64_t t_ctxt_leaves_count = 0;
      if (t_term->type() == ir::Term::Type::cipher)
      {
        count_ctxt_leaves(t_term, dp);
        t_ctxt_leaves_count = dp.at(t_term);
      }

      return t_ctxt_leaves_count - x_ctxt_leaves_count - y_ctxt_leaves_count < 0;
    };
  };

  vector<Rule> add_rules{
    {"assoc-balan-add-1-1", ((x + y) + z) + t, (x + y) + (z + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-1-2", (z + (x + y)) + t, (z + x) + (y + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-2-1", x + (y + (z + t)), (x + y) + (z + t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-2-2", x + ((z + t) + y), (x + z) + (t + y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-1-1-1", ((x + y) - z) + t, (x + y) - (z - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-1-1-2", (z - (x + y)) + t, (z - x) - (y - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-1-2-1", ((x - y) + z) + t, (x - y) + (z + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-1-2-2", (z + (x - y)) + t, (z + x) - (y - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-1-3-1", ((x - y) - z) + t, (x - y) - (z - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-1-3-2", (z - (x - y)) + t, (z - x) + (y + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-add-sub-2-1-1", x + (y - (z + t)), (x + y) - (z + t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-2-1-2", x + ((z + t) - y), (x + z) + (t - y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-2-2-1", x + (y + (z - t)), (x + y) + (z - t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-2-2-2", x + ((z - t) + y), (x + z) - (t - y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-2-3-1", x + (y - (z - t)), (x + y) - (z - t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-add-sub-2-3-2", x + ((z - t) - y), (x + z) - (t + y), assoc_balan_cond(z, t, x)}};

  vector<Rule> sub_rules{
    {"assoc-balan-sub-1-1", ((x - y) - z) - t, (x - y) - (z + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-1-2", (z - (x - y)) - t, (z - x) + (y - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-2-1", x - (y - (z - t)), (x - y) - (z + t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-2-2", x - ((z - t) - y), (x - z) + (t + y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-1-1-1", ((x - y) + z) - t, (x - y) + (z - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-1-1-2", (z + (x - y)) - t, (z + x) - (y + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-1-2-1", ((x + y) - z) - t, (x + y) - (z + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-1-2-2", (z - (x + y)) - t, (z - x) - (y + t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-1-3-1", ((x + y) + z) - t, (x + y) + (z - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-1-3-2", (z + (x + y)) - t, (z + x) + (y - t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-sub-add-2-1-1", x - (y + (z - t)), (x - y) - (z - t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-2-1-2", x - ((z - t) + y), (x - z) + (t - y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-2-2-1", x - (y - (z + t)), (x - y) + (z + t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-2-2-2", x - ((z + t) - y), (x - z) - (t - y), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-2-3-1", x - (y + (z + t)), (x - y) - (z + t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-sub-add-2-3-2", x - ((z + t) + y), (x - z) - (t + y), assoc_balan_cond(z, t, x)}};

  vector<Rule> mul_rules{
    {"assoc-balan-mul-square-1-1", ((x * y) * x) * z, (x * x) * (y * z), assoc_balan_cond(x, y, z)},
    {"assoc-balan-mul-square-1-2", (x * (x * y)) * z, (x * x) * (y * z), assoc_balan_cond(x, y, z)},
    {"assoc-balan-mul-square-2-1", ((x * y) * y) * z, (x * z) * (y * y), assoc_balan_cond(x, y, z)},
    {"assoc-balan-mul-square-2-2", (y * (x * y)) * z, (y * y) * (x * z), assoc_balan_cond(x, y, z)},
    {"assoc-balan-mul-square-3-1", ((x * y) * z) * x, (x * x) * (y * z)},
    {"assoc-balan-mul-square-3-2", x * ((x * y) * z), (x * x) * (y * z)},
    {"assoc-balan-mul-square-4-1", ((x * y) * z) * y, (x * z) * (y * y)},
    {"assoc-balan-mul-square-4-2", y * ((x * y) * z), (y * y) * (x * z)},
    {"assoc-balan-mul-1-1", ((x * y) * z) * t, (x * y) * (z * t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-mul-1-2", (z * (x * y)) * t, (z * x) * (y * t), assoc_balan_cond(x, y, t)},
    {"assoc-balan-mul-2-1", x * (y * (z * t)), (x * y) * (z * t), assoc_balan_cond(z, t, x)},
    {"assoc-balan-mul-2-2", x * ((z * t) * y), (x * z) * (t * y), assoc_balan_cond(z, t, x)}};

  return Ruleset{slot_count, move(add_rules), move(sub_rules), {}, {}, {}, move(mul_rules)};
}

Ruleset Ruleset::log2_reduct_prep_ruleset(std::size_t slot_count)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  vector<Rule> add_rules{{"merge-rotate-add", (x << n) + (y << n), (x + y) << n}};

  vector<Rule> sub_rules{{"merge-rotate-sub", (x << n) - (y << n), (x - y) << n}};

  vector<Rule> rotate_rules{{"rotate0", x << 0, x}, {"fold-rotate", x << n << m, x << ((m + n) % slot_count)}};

  vector<Rule> mul_rules{{"merge-rotate-mul", (x << n) * (y << n), (x * y) << n}};

  return Ruleset{slot_count, move(add_rules), move(sub_rules), {}, {move(rotate_rules)}, {}, move(mul_rules)};
}

Ruleset Ruleset::log2_reduct_opt_ruleset(std::size_t slot_count)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  vector<Rule> add_rules = get_log_reduct_rules(slot_count, x, TermOpCode::add);
  vector<Rule> add_rules_rotate{
    {"nest-rotate-add-1", (x << n) + (y << m), (x + (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-add-2", (x << n) + (y << m), ((x << (n - m)) + y) << m, [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }}};
  add_rules.insert(add_rules.end(), add_rules_rotate.begin(), add_rules_rotate.end());

  vector<Rule> sub_rules = get_log_reduct_rules(slot_count, x, TermOpCode::sub);
  vector<Rule> sub_rules_rotate{
    {"nest-rotate-sub-1", (x << n) - (y << m), (x - (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-sub-2", (x << n) - (y << m), ((x << (n - m)) - y) << m, [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }}};
  sub_rules.insert(sub_rules.end(), sub_rules_rotate.begin(), sub_rules_rotate.end());

  vector<Rule> mul_rules = get_log_reduct_rules(slot_count, x, TermOpCode::mul);
  vector<Rule> mul_rules_rotate{
    {"nest-rotate-mul-1", (x << n) * (y << m), (x * (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-mul-2", (x << n) * (y << m), ((x << (n - m)) * y) << m, [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }}};
  mul_rules.insert(mul_rules.end(), mul_rules_rotate.begin(), mul_rules_rotate.end());

  return Ruleset{slot_count, move(add_rules), move(sub_rules), {}, {}, {}, move(mul_rules)};
}

Ruleset Ruleset::ops_type_number_opt_ruleset(size_t slot_count)
{
  TermMatcher x{TermMatcherType::term, "x"};
  TermMatcher y{TermMatcherType::term, "y"};
  TermMatcher z{TermMatcherType::term, "z"};

  TermMatcher const1{TermMatcherType::const_, "const1"};
  TermMatcher const2{TermMatcherType::const_, "const2"};
  TermMatcher zero{"zero", PackedVal(slot_count, 0)};
  TermMatcher one{"one", PackedVal(slot_count, 1)};
  TermMatcher m_one{"m_one", PackedVal(slot_count, -1)};
  TermMatcher two{"two", PackedVal(slot_count, 2)};

  TermMatcher c_x{TermMatcherType::cipher, "ctxt_x"};
  TermMatcher c_y{TermMatcherType::cipher, "ctxt_y"};
  TermMatcher c_z{TermMatcherType::cipher, "ctxt_z"};

  TermMatcher p_x{TermMatcherType::plain, "ptxt_x"};
  TermMatcher p_y{TermMatcherType::plain, "ptxt_y"};
  TermMatcher p_z{TermMatcherType::plain, "ptxt_z"};

  OpGenMatcher n{"n"};
  OpGenMatcher m{"m"};

  vector<Rule> add_rules{
    {"add0-1", x + zero, x},
    {"add0-2", zero + x, x},
    {"part-fold-assoc-add-1", const1 + (const2 + x), (const1 + const2) + x},
    {"part-fold-assoc-add-2", const1 + (x + const2), (const1 + const2) + x},
    {"part-fold-assoc-add-3", (const1 + x) + const2, x + (const1 + const2)},
    {"part-fold-assoc-add-4", (x + const1) + const2, x + (const1 + const2)},
    {"part-fold-assoc-add-sub-1", const1 + (const2 - x), (const1 + const2) - x},
    {"part-fold-assoc-add-sub-2", const1 + (x - const2), (const1 - const2) + x},
    {"part-fold-assoc-add-sub-3", (const1 - x) + const2, -x + (const1 + const2)},
    {"part-fold-assoc-add-sub-4", (x - const1) + const2, x + (-const1 + const2)},
    // {"fold-add-negate-1", x + (-y), x - y},
    // {"fold-add-negate-2", (-y) + x, x - y},
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
    {"fact-add-1-3", p_x * p_y + p_z * p_y, (p_x + p_z) * p_y},
    {"fact-add-2-1", x * c_y + c_y * z, (x + z) * c_y},
    {"fact-add-2-2", c_x * p_y + p_y * c_z, (c_x + c_z) * p_y},
    {"fact-add-2-3", p_x * p_y + p_y * p_z, (p_x + p_z) * p_y},
    {"fact-add-3-1", c_y * x + z * c_y, c_y * (x + z)},
    {"fact-add-3-2", p_y * c_x + c_z * p_y, p_y * (c_x + c_z)},
    {"fact-add-3-3", p_y * p_x + p_z * p_y, p_y * (p_x + p_z)},
    {"fact-add-4-1", c_y * x + c_y * z, c_y * (x + z)},
    {"fact-add-4-2", p_y * c_x + p_y * c_z, p_y * (c_x + c_z)},
    {"fact-add-4-3", p_y * p_x + p_y * p_z, p_y * (p_x + p_z)},
    // {"fact-add-mul1-1", x + x * y, x * (one + y)},
    // {"fact-add-mul1-2", x + y * x, (one + y) * x},
    // {"fact-add-mul1-3", x * y + x, x * (y + one)},
    // {"fact-add-mul1-4", y * x + x, (y + one) * x},
    // {"ident-1-1", square(x) + square(y) + (two * x) * y, square(x + y)},
    // {"ident-1-2", square(x) + square(y) + (two * y) * x, square(x + y)},
    // {"ident-1-3", square(x) + square(y) + two * (x * y), square(x + y)},
    // {"ident-1-4", square(x) + square(y) + two * (y * x), square(x + y)},
    // {"ident-1-5", square(x) + square(y) + (x * two) * y, square(x + y)},
    // {"ident-1-6", square(x) + square(y) + (y * two) * x, square(x + y)},
    // {"ident-1-7", square(x) + square(y) + x * (two * y), square(x + y)},
    // {"ident-1-8", square(x) + square(y) + y * (two * x), square(x + y)},
    // {"ident-1-9", square(x) + square(y) + (x * y) * two, square(x + y)},
    // {"ident-1-10", square(x) + square(y) + (y * x) * two, square(x + y)},
    // {"ident-1-11", square(x) + square(y) + x * (y * two), square(x + y)},
    // {"ident-1-12", square(x) + square(y) + y * (x * two), square(x + y)},
  };
  // auto log_add_reduct_rules = get_log_reduct_rules(x, TermOpCode::add);
  // add_rules_.insert(add_rules_.end(), log_add_reduct_rules.begin(), log_add_reduct_rules.end());

  vector<Rule> sub_rules{
    {"sub0-1", x - zero, x},
    {"sub0-2", zero - x, -x},
    {"subx-x", x - x, zero},
    {"part-fold-assoc-sub-1", const1 - (const2 - x), (const1 - const2) - x},
    {"part-fold-assoc-sub-2", const1 - (x - const2), (const1 - const2) - x},
    {"part-fold-assoc-sub-3", (const1 - x) - const2, x - (const1 - const2)},
    {"part-fold-assoc-sub-4", (x - const1) - const2, x - (const1 - const2)},
    {"part-fold-assoc-sub-add-1", const1 - (const2 + x), (const1 - const2) - x},
    {"part-fold-assoc-sub-add-2", const1 - (x + const2), (const1 - const2) + x},
    {"part-fold-assoc-sub-add-3", (const1 + x) - const2, x + (const1 - const2)},
    {"part-fold-assoc-sub-add-4", (x + const1) - const2, x + (const1 - const2)},
    // {"fold-sub-negate-1", x - (-y), x + y},
    // {"fold-sub-negate-2", (-y) - x, x + y},
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
    {"fact-sub-1-3", p_x * p_y - p_z * p_y, (p_x - p_z) * p_y},
    {"fact-sub-2-1", x * c_y - c_y * z, (x - z) * c_y},
    {"fact-sub-2-2", c_x * p_y - p_y * c_z, (c_x - c_z) * p_y},
    {"fact-sub-2-3", p_x * p_y - p_y * p_z, (p_x - p_z) * p_y},
    {"fact-sub-3-1", c_y * x - z * c_y, c_y * (x - z)},
    {"fact-sub-3-2", p_y * c_x - c_z * p_y, p_y * (c_x - c_z)},
    {"fact-sub-3-3", p_y * p_x - p_z * p_y, p_y * (p_x - p_z)},
    {"fact-sub-4-1", c_y * x - c_y * z, c_y * (x - z)},
    {"fact-sub-4-2", p_y * c_x - p_y * c_z, p_y * (c_x - c_z)},
    {"fact-sub-4-3", p_y * p_x - p_y * p_z, p_y * (p_x - p_z)},
    // {"fact-sub-mul1", x - x * y, x * (one - y)},
    // {"fact-sub-mul2", x - y * x, (one - y) * x},
    // {"fact-sub-mul3", x * y - x, x * (y - one)},
    // {"fact-sub-mul4", y * x - x, (y - one) * x},
    // {"ident-2-1", square(x) + square(y) - (two * x) * y, square(x - y)},
    // {"ident-2-2", square(x) + square(y) - (two * y) * x, square(x - y)},
    // {"ident-2-3", square(x) + square(y) - two * (x * y), square(x - y)},
    // {"ident-2-4", square(x) + square(y) - two * (y * x), square(x - y)},
    // {"ident-2-5", square(x) + square(y) - (x * two) * y, square(x - y)},
    // {"ident-2-6", square(x) + square(y) - (y * two) * x, square(x - y)},
    // {"ident-2-7", square(x) + square(y) - x * (two * y), square(x - y)},
    // {"ident-2-8", square(x) + square(y) - y * (two * x), square(x - y)},
    // {"ident-2-9", square(x) + square(y) - (x * y) * two, square(x - y)},
    // {"ident-2-10", square(x) + square(y) - (y * x) * two, square(x - y)},
    // {"ident-2-11", square(x) + square(y) - x * (y * two), square(x - y)},
    // {"ident-2-12", square(x) + square(y) - y * (x * two), square(x - y)},
    // {"ident-3", square(x) - square(y), (x + y) * (x - y)},
  };

  vector<Rule> negate_rules{
    {"fold-negate", -(-x), x}, {"fold-negate-add", -(-x + y), x - y}, {"fold-negate-sub", -(-x - y), x + y}};

  vector<Rule> rotate_rules{
    {"rotate0", x << 0, x},
    {"fold-rotate", x << n << m, x << ((m + n) % slot_count)},
    {"part-fold-rotate-add-1", (x + const1) << n, (x << n) + (const1 << n)},
    {"part-fold-rotate-add-2", (const1 + x) << n, (const1 << n) + (x << n)},
    {"part-fold-rotate-sub-1", (x - const1) << n, (x << n) - (const1 << n)},
    {"part-fold-rotate-sub-2", (const1 - x) << n, (const1 << n) - (x << n)},
    {"part-fold-rotate-mul-1", (x * const1) << n, (x << n) * (const1 << n)},
    {"part-fold-rotate-mul-2", (const1 * x) << n, (const1 << n) * (x << n)},
    {"rev-nest-rotate-add-1", (x + (y << n)) << m, (x << m) + y,
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-add-2", ((x << n) + y) << m, x + (y << m),
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-sub-1", (x - (y << n)) << m, (x << m) - y,
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-sub-2", ((x << n) - y) << m, x - (y << m),
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-mul-1", (x * (y << n)) << m, (x << m) * y,
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
    {"rev-nest-rotate-mul-2", ((x << n) * y) << m, x * (y << m),
     [n, m, slot_count](const Subst subst) {
       return (subst.get(n) + subst.get(m)) % slot_count == 0;
     }},
  };

  vector<Rule> mul_rules{
    {"mul0-1", x * zero, zero},
    {"mul0-2", zero * x, zero},
    {"mul1-1", x * one, x},
    {"mul1-2", one * x, x},
    {"mul-m1-1", x * m_one, -x},
    {"mul-m1-2", m_one * x, -x},
    {"mul-to-square", x * x, square(x)},
    {"part-fold-dist-add-1", const1 * (const2 + x), const1 * const2 + const1 * x},
    {"part-fold-dist-add-2", const1 * (x + const2), const1 * x + const1 * const2},
    {"part-fold-dist-add-3", (const1 + x) * const2, const1 * const2 + x * const2},
    {"part-fold-dist-add-4", (x + const1) * const2, x * const2 + const1 * const2},
    {"part-fold-dist-sub-1", const1 * (const2 - x), const1 * const2 - const1 * x},
    {"part-fold-dist-sub-2", const1 * (x - const2), const1 * x - const1 * const2},
    {"part-fold-dist-sub-3", (const1 - x) * const2, const1 * const2 - x * const2},
    {"part-fold-dist-sub-4", (x - const1) * const2, x * const2 - const1 * const2},
    {"part-fold-dist-sub-4", (x - const1) * const2, x * const2 - const1 * const2},
    {"part-fold-assoc-mul-1", const1 * (const2 * x), (const1 * const2) * x},
    {"part-fold-assoc-mul-2", const1 * (x * const2), (const1 * const2) * x},
    {"part-fold-assoc-mul-3", (const1 * x) * const2, x * (const1 * const2)},
    {"part-fold-assoc-mul-4", (x * const1) * const2, x * (const1 * const2)},
    {"merge-rotate-mul", (x << n) * (y << n), (x * y) << n},
    {"nest-rotate-mul-1", (x << n) * (y << m), (x * (y << (m - n))) << n,
     [n, m](const Subst subst) {
       return subst.get(n) < subst.get(m);
     }},
    {"nest-rotate-mul-2", (x << n) * (y << m), ((x << (n - m)) * y) << m, [n, m](const Subst subst) {
       return subst.get(n) > subst.get(m);
     }}};

  return Ruleset{slot_count, move(add_rules), move(sub_rules), move(negate_rules), move(rotate_rules), {}, mul_rules};
}

Ruleset::Ruleset(size_t slot_count, vector<Rule> rules)
{
  for (auto &rule : rules)
  {
    switch (rule.lhs().op_code().type())
    {
    case ir::OpCode::Type::nop:
      throw invalid_argument("rule lhs is a leaf, no context!");

    case ir::OpCode::Type::add:
      add_rules_.push_back(move(rule));
      break;

    case ir::OpCode::Type::sub:
      sub_rules_.push_back(move(rule));
      break;

    case ir::OpCode::Type::negate:
      negate_rules_.push_back(move(rule));
      break;

    case ir::OpCode::Type::rotate:
      rotate_rules_.push_back(move(rule));
      break;

    case ir::OpCode::Type::square:
      square_rules_.push_back(move(rule));
      break;

    case ir::OpCode::Type::mul:
      mul_rules_.push_back(move(rule));
      break;

    default:
      throw logic_error("unhandled operation of rule lhs");
    }
  }
}

const vector<Rule> &Ruleset::pick_rules(const ir::OpCode &op_code) const
{
  switch (op_code.type())
  {
  case ir::OpCode::Type::nop:
    throw invalid_argument("cannot pick rules for nop");

  case ir::OpCode::Type::add:
    return add_rules_;

  case ir::OpCode::Type::sub:
    return sub_rules_;

  case ir::OpCode::Type::negate:
    return negate_rules_;

  case ir::OpCode::Type::rotate:
    return rotate_rules_;

  case ir::OpCode::Type::square:
    return square_rules_;

  case ir::OpCode::Type::mul:
    return mul_rules_;

  default:
    throw logic_error("unhandled pick_rules for op_code");
  }
}

vector<Rule> Ruleset::get_log_reduct_rules(size_t slot_count, const TermMatcher &x, const TermOpCode &op_code)
{
  vector<Rule> rules;
  if (!util::is_power_of_two(slot_count))
    slot_count = util::next_power_of_two(slot_count >> 1);
  while (slot_count > 1)
  {
    rules.push_back(make_log_reduct_comp(x, slot_count, op_code));
    slot_count >>= 1;
  }
  return rules;
}

Rule Ruleset::make_log_reduct_comp(const TermMatcher &x, size_t size, const TermOpCode &op_code)
{
  if (!util::is_power_of_two(size))
    throw invalid_argument("make_log_reduct_comp size must be a power of two");

  if (op_code.arity() != 2)
    throw invalid_argument("binary operation assumed for reduction");

  vector<TermMatcher> elts;
  elts.reserve(size);
  size_t largest_power2 = 1;
  elts.push_back(x);
  for (size_t i = 1; i < size; ++i)
  {
    if (i != 0 && (i & (i - 1)) == 0)
      largest_power2 = i;

    elts.push_back(x << i);
  }
  auto lhs = balanced_op(elts, op_code);

  vector<TermMatcher> sorted_elts;
  sorted_elts.reserve(size);
  for (size_t j = 0; j < largest_power2; ++j)
  {
    sorted_elts.push_back(elts[j]);
    sorted_elts.push_back(elts[j + largest_power2]);
  }
  auto rhs = balanced_op(sorted_elts, op_code);

  return Rule{"log-reduct-" + op_code.str_repr() + "-" + to_string(size), lhs, rhs};
}
} // namespace fheco::trs
