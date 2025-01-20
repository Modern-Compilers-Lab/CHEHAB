#include "fheco/ir/func.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/ruleset.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/expr_printer.hpp"
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <iostream> 
using namespace std;

namespace fheco::trs
{
void print_ruleset(const Ruleset &ruleset, ostream &os)
{
  os << ruleset.name() << ":\n";
  for (const auto &[root_op_type, rules] : ruleset.rules_by_root_op())
  {
    os << "\"" << root_op_type << "\" rules:\n";
    for (const auto &rule : rules)
      os << util::ExprPrinter::make_rule_str_repr(rule) << '\n';
  }
}
/*************************************************************************/
Ruleset Ruleset::SumVec_reduct_opt_ruleset(shared_ptr<ir::Func> func)
{
  vector<ir::OpCode::Type> binary_ops{ir::OpCode::Type::SumVec};
  TermMatcher x{TermMatcherType::term, "x"};
  RulesByRootOp rules_by_root_op;
  size_t slot_count = func->slot_count() ;
  vector<Rule> rules;
  for (const auto &op : binary_ops){
    if (!util::is_power_of_two(slot_count))
      slot_count = util::next_power_of_two(slot_count >> 1); // 4 >> 1 =2
    while (slot_count > 2)
    {
      rules.push_back(make_SumVec_reduct_comp(x, TermOpCode::SumVec(slot_count),slot_count));
      slot_count >>= 1;
    }
    rules_by_root_op.emplace(op,rules);
  }
  return Ruleset{move(func), "SumVec_reduct_opt_ruleset", move(rules_by_root_op)};
}
/************************************************************************************/
Rule Ruleset::make_SumVec_reduct_comp(const TermMatcher &x, TermOpCode op_code,size_t size)
{
  if (!util::is_power_of_two(size))
    throw invalid_argument("make_log_reduct_comp size must be a power of two");

  TermMatcher TermTemp{op_code, vector<TermMatcher>{x}} ;
  auto lhs = TermTemp ;
  auto prev_elt = x ;
  size_t prev_power2 = size >> 1;
  auto elt = x + (x << prev_power2);
  while (prev_power2 > 1) {
      prev_power2 = prev_power2 >> 1 ;
      elt= elt + (elt << prev_power2) ;
  }
  auto rhs = elt ;
  return Rule{"reduct-" + op_code.str_repr(), lhs, rhs};
}
/*************************************************************************************/
Ruleset Ruleset::log2_reduct_opt_ruleset(shared_ptr<ir::Func> func)
{
    /*                          
      enum class TermMatcherType
      {
        cipher,
        term,
        plain,
        const_
      };
    */
    /* TermMatcher 
    {
      static std::size_t count_;
      std::size_t id_;
      TermOpCode op_code_;
      std::vector<TermMatcher> operands_;
      TermMatcherType type_;
      std::optional<std::string> label_;
      std::optional<PackedVal> val_;
    }
    */
  vector<TermOpCode> binary_ops{TermOpCode::add, TermOpCode::sub, TermOpCode::mul};
  TermMatcher x{TermMatcherType::term, "x"};
  RulesByRootOp rules_by_root_op;
  for (const auto &op : binary_ops)
    rules_by_root_op.emplace(op.type(), get_log_reduct_rules(x, op, func->slot_count()));
  return Ruleset{move(func), "log2_reduct_opt_ruleset", move(rules_by_root_op)};
}
/***************************************************************************************************/
/***************************************************************************************************/
Ruleset Ruleset::customize_generic_rules(const Ruleset &ruleset)
{
  RulesByRootOp rules_by_root_op;
  for (const auto &[root_op_type, rules] : ruleset.rules_by_root_op())
  {
    vector<Rule> customized_op_rules;
    for (const auto &rule : rules)
    {
      if (rule.lhs().type() == TermMatcherType::term && !rule.has_dynamic_rhs())
      {
        for (auto &variant : rule.generate_customized_terms_variants())
          customized_op_rules.push_back(move(variant));
      }
      else
        customized_op_rules.push_back(rule);
    }
    rules_by_root_op.emplace(root_op_type, move(customized_op_rules));
  }
  return Ruleset{ruleset.func(), ruleset.name() + "_customized_generic_rules", move(rules_by_root_op)};
}
/**********************************************************************************************************/
/**********************************************************************************************************/
Ruleset::Ruleset(
  shared_ptr<ir::Func> func, string name, vector<Rule> rules, unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp)
  : func_{move(func)}, name_{move(name)}, rules_by_root_op_{},
    terms_ctxt_leaves_count_dp_{move(terms_ctxt_leaves_count_dp)}
{
  for (auto &rule : rules)
  {
    auto root_op_type = rule.lhs().op_code().type();
    if (auto it = rules_by_root_op_.find(root_op_type); it != rules_by_root_op_.end())
      it->second.push_back(move(rule));
    else
      rules_by_root_op_.emplace(root_op_type, vector<Rule>{move(rule)});
  }
}
/*****************************************************************************************/
/*****************************************************************************************/
vector<Rule> Ruleset::get_log_reduct_rules(const TermMatcher &x, const TermOpCode &op_code, size_t slot_count)
{
  vector<Rule> rules;
  if (!util::is_power_of_two(slot_count))
    slot_count = util::next_power_of_two(slot_count >> 1); // 4 >> 1 =2
  while (slot_count > 2)
  {
    rules.push_back(make_log_reduct_comp(x, op_code, slot_count));
    slot_count >>= 1;
  }
  return rules;
}
/******************************************************************************************/
/******************************************************************************************/
const Rule &Ruleset::get_rule(const string &name) const
{
  for (const auto &[op_code_type, rules] : rules_by_root_op_)
  {
    try
    {
      return get_rule(name, op_code_type);
    }
    catch (const invalid_argument &e)
    {}
  }
  throw invalid_argument("no rule with name was found");
}

const Rule &Ruleset::get_rule(const string &name, ir::OpCode::Type op_code_type) const
{
  for (const auto &rule : rules_by_root_op_.at(op_code_type))
  {
    if (rule.name() == name)
      return rule;
  }
  throw invalid_argument("no rule with name was found");
}
/************************************************************************************************/
Rule Ruleset::make_log_reduct_comp(const TermMatcher &x, const TermOpCode &op_code, size_t size)
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
  //auto lhs = non_balanced_op(lhs_elts, op_code);
  auto lhs = balanced_op(lhs_elts, op_code);
  size_t prev_power2 = size >> 1;
  vector<TermMatcher> rhs_elts;
  rhs_elts.reserve(size);
  /**************/
  auto basic_elt = x + (x << prev_power2);
  rhs_elts.push_back(basic_elt);
  for (size_t i = 1; i < prev_power2; ++i)
    rhs_elts.push_back(basic_elt << i);
  auto rhs = balanced_op(rhs_elts, op_code);
  /********************************************
  auto prev_elt = x ;
  auto elt = x + (x << prev_power2);
  while (prev_power2 > 1) {
      prev_power2 = prev_power2 >> 1 ;
      elt= elt + (elt << prev_power2) ;
  }
  auto rhs = elt ;
  /*********************************************/
  return Rule{"log-reduct-" + op_code.str_repr() + "-" + to_string(size), lhs, rhs};
}
/****************************************************************************************/
bool operator==(const Ruleset &lhs, const Ruleset &rhs)
{
  return *lhs.func() == *rhs.func() && lhs.rules_by_root_op() == rhs.rules_by_root_op();
}

Ruleset operator&(const Ruleset &lhs, const Ruleset &rhs)
{
  if (*lhs.func() != *rhs.func())
    throw invalid_argument("lhs and rhs rulesets are associated with different functions (not in the same domain)");

  unordered_map<ir::OpCode::Type, unordered_set<string>> rhs_rules_names_by_root_op;
  for (const auto &[root_op_type, rules] : rhs.rules_by_root_op())
  {
    auto it = rhs_rules_names_by_root_op.emplace(root_op_type, unordered_set<string>{}).first;
    for (const auto &rule : rules)
      it->second.emplace(rule.name());
  }

  Ruleset::RulesByRootOp rules_by_root_op;
  for (const auto &[root_op_type, rules] : lhs.rules_by_root_op())
  {
    auto rhs_op_rules_names_it = rhs_rules_names_by_root_op.find(root_op_type);
    if (rhs_op_rules_names_it == rhs_rules_names_by_root_op.end())
      continue;

    vector<Rule> op_common_rules;
    for (const auto &rule : rules)
    {
      if (rhs_op_rules_names_it->second.find(rule.name()) != rhs_op_rules_names_it->second.end())
        op_common_rules.push_back(rule);
    }
    rules_by_root_op.emplace(root_op_type, move(op_common_rules));
  }
  return Ruleset{lhs.func(), lhs.name() + " & " + rhs.name(), move(rules_by_root_op)};
}

ostream &operator<<(ostream &os, const Ruleset &ruleset)
{
  os << ruleset.name() << '\n';
  size_t total = 0;
  for (const auto &[root_op_type, rules] : ruleset.rules_by_root_op())
  {
    os << root_op_type << ": " << rules.size() << '\n';
    total += rules.size();
  }
  os << "total: " << total << '\n';
  return os;
}
} // namespace fheco::trs
