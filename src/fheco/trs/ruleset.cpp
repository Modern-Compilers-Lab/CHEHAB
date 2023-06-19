#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/util/common.hpp"
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;

namespace fheco::trs
{
Ruleset Ruleset::log2_reduct_opt_ruleset(shared_ptr<ir::Func> func)
{
  TermMatcher x{TermMatcherType::term, "x"};
  vector<Rule> add_rules = get_log_reduct_rules(func->slot_count(), x, TermOpCode::add);
  vector<Rule> sub_rules = get_log_reduct_rules(func->slot_count(), x, TermOpCode::sub);
  vector<Rule> mul_rules = get_log_reduct_rules(func->slot_count(), x, TermOpCode::mul);
  return Ruleset{move(func), "log2_reduct_opt_ruleset", move(add_rules), move(sub_rules), {}, {}, {}, move(mul_rules)};
}

Ruleset Ruleset::customize_generic_rules(const Ruleset &ruleset)
{
  vector<Rule> add_rules;
  for (const auto &rule : ruleset.add_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        add_rules.push_back(move(variant));
    }
    else
      add_rules.push_back(move(rule));
  }

  vector<Rule> sub_rules;
  for (const auto &rule : ruleset.sub_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        sub_rules.push_back(move(variant));
    }
    else
      sub_rules.push_back(move(rule));
  }

  vector<Rule> negate_rules;
  for (const auto &rule : ruleset.negate_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        negate_rules.push_back(move(variant));
    }
    else
      negate_rules.push_back(move(rule));
  }

  vector<Rule> rotate_rules;
  for (const auto &rule : ruleset.rotate_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        rotate_rules.push_back(move(variant));
    }
    else
      rotate_rules.push_back(move(rule));
  }

  vector<Rule> square_rules;
  for (const auto &rule : ruleset.square_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        square_rules.push_back(move(variant));
    }
    else
      square_rules.push_back(move(rule));
  }

  vector<Rule> mul_rules;
  for (const auto &rule : ruleset.mul_rules())
  {
    if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
    {
      for (auto &variant : rule.generate_customized_terms_variants())
        mul_rules.push_back(move(variant));
    }
    else
      mul_rules.push_back(move(rule));
  }

  return Ruleset{ruleset.func(),     ruleset.name() + "_customized_generic_rules",
                 move(add_rules),    move(sub_rules),
                 move(negate_rules), move(rotate_rules),
                 move(square_rules), move(mul_rules)};
}

Ruleset::Ruleset(
  shared_ptr<ir::Func> func, string name, vector<Rule> rules, unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp)
  : func_{move(func)}, name_{move(name)}, terms_ctxt_leaves_count_dp_{move(terms_ctxt_leaves_count_dp)}
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
  while (slot_count > 2)
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

  vector<TermMatcher> lhs_elts;
  lhs_elts.reserve(size);
  lhs_elts.push_back(x);
  for (size_t i = 1; i < size; ++i)
    lhs_elts.push_back(x << i);
  auto lhs = balanced_op(lhs_elts, op_code);

  size_t prev_power2 = size >> 1;
  vector<TermMatcher> rhs_elts;
  rhs_elts.reserve(size);
  auto basic_elt = x + (x << prev_power2);
  rhs_elts.push_back(basic_elt);
  for (size_t i = 1; i < prev_power2; ++i)
    rhs_elts.push_back(basic_elt << i);
  auto rhs = balanced_op(rhs_elts, op_code);

  return Rule{"log-reduct-" + op_code.str_repr() + "-" + to_string(size), lhs, rhs};
}

bool operator==(const Ruleset &lhs, const Ruleset &rhs)
{
  return *lhs.func() == *rhs.func() && lhs.add_rules() == rhs.add_rules() && lhs.sub_rules() == rhs.sub_rules() &&
         lhs.negate_rules() == rhs.negate_rules() && lhs.rotate_rules() == rhs.rotate_rules() &&
         lhs.square_rules() == rhs.square_rules() && lhs.mul_rules() == rhs.mul_rules();
}

Ruleset operator&(const Ruleset &lhs, const Ruleset &rhs)
{
  if (*lhs.func() != *rhs.func())
    throw invalid_argument("lhs and rhs rulesets are associated with different functions (not in the same domain)");

  unordered_set<string> rhs_add_rules;
  for (const auto &rule : rhs.add_rules())
    rhs_add_rules.insert(rule.name());
  vector<Rule> add_rules;
  for (const auto &rule : lhs.add_rules())
  {
    if (rhs_add_rules.find(rule.name()) != rhs_add_rules.end())
      add_rules.push_back(rule);
  }

  unordered_set<string> rhs_sub_rules;
  for (const auto &rule : rhs.sub_rules())
    rhs_sub_rules.insert(rule.name());
  vector<Rule> sub_rules;
  for (const auto &rule : lhs.sub_rules())
  {
    if (rhs_sub_rules.find(rule.name()) != rhs_sub_rules.end())
      sub_rules.push_back(rule);
  }

  unordered_set<string> rhs_negate_rules;
  for (const auto &rule : rhs.negate_rules())
    rhs_negate_rules.insert(rule.name());
  vector<Rule> negate_rules;
  for (const auto &rule : lhs.negate_rules())
  {
    if (rhs_negate_rules.find(rule.name()) != rhs_negate_rules.end())
      negate_rules.push_back(rule);
  }

  unordered_set<string> rhs_rotate_rules;
  for (const auto &rule : rhs.rotate_rules())
    rhs_rotate_rules.insert(rule.name());
  vector<Rule> rotate_rules;
  for (const auto &rule : lhs.rotate_rules())
  {
    if (rhs_rotate_rules.find(rule.name()) != rhs_rotate_rules.end())
      rotate_rules.push_back(rule);
  }

  unordered_set<string> rhs_square_rules;
  for (const auto &rule : rhs.square_rules())
    rhs_square_rules.insert(rule.name());
  vector<Rule> square_rules;
  for (const auto &rule : lhs.square_rules())
  {
    if (rhs_square_rules.find(rule.name()) != rhs_square_rules.end())
      square_rules.push_back(rule);
  }

  unordered_set<string> rhs_mul_rules;
  for (const auto &rule : rhs.mul_rules())
    rhs_mul_rules.insert(rule.name());
  vector<Rule> mul_rules;
  for (const auto &rule : lhs.mul_rules())
  {
    if (rhs_mul_rules.find(rule.name()) != rhs_mul_rules.end())
      mul_rules.push_back(rule);
  }

  return Ruleset{lhs.func(),         lhs.name() + " & " + rhs.name(),
                 move(add_rules),    move(sub_rules),
                 move(negate_rules), move(rotate_rules),
                 move(square_rules), move(mul_rules)};
}

ostream &operator<<(ostream &os, const Ruleset &ruleset)
{
  os << ruleset.name() << '\n';
  os << "add: " << ruleset.add_rules().size() << '\n';
  os << "sub: " << ruleset.sub_rules().size() << '\n';
  os << "negate: " << ruleset.negate_rules().size() << '\n';
  os << "rotate: " << ruleset.rotate_rules().size() << '\n';
  os << "square: " << ruleset.square_rules().size() << '\n';
  os << "mul: " << ruleset.mul_rules().size() << '\n';
  size_t total = ruleset.add_rules().size() + ruleset.sub_rules().size() + ruleset.negate_rules().size() +
                 ruleset.rotate_rules().size() + ruleset.square_rules().size() + ruleset.mul_rules().size();
  os << "total: " << total << '\n';
  return os;
}
} // namespace fheco::trs
