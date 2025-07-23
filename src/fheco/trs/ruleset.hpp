#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
 
namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::trs
{
class Ruleset
{
public:
  using RulesByRootOp = std::unordered_map<ir::OpCode::Type, std::vector<Rule>>;

  static Ruleset simplification_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset depth_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset ops_cost_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset joined_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset depth_after_simplify_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset halide_adapted_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset log2_reduct_opt_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset SumVec_reduct_opt_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset customize_generic_rules(const Ruleset &ruleset);

  Ruleset(
    std::shared_ptr<ir::Func> func, std::string name, RulesByRootOp rules_by_root_op,
    std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp = nullptr)
    : func_{std::move(func)}, name_{std::move(name)}, rules_by_root_op_{std::move(rules_by_root_op)},
      terms_ctxt_leaves_count_dp_{std::move(terms_ctxt_leaves_count_dp)}
  {}

  Ruleset(
    std::shared_ptr<ir::Func> func, std::string name, std::vector<Rule> rules,
    std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp = nullptr);

  inline const std::vector<Rule> &pick_rules(const ir::OpCode::Type &op_code_type)
  {
    return rules_by_root_op_[op_code_type];
  }

  const Rule &get_rule(const std::string &name) const;

  const Rule &get_rule(const std::string &name, ir::OpCode::Type op_code_type) const;

  inline const std::shared_ptr<ir::Func> &func() const { return func_; }

  inline std::string name() const { return name_; }

  inline const RulesByRootOp &rules_by_root_op() const { return rules_by_root_op_; }

private:
  static std::vector<Rule> get_log_reduct_rules(
    const TermMatcher &x, const TermOpCode &op_code, std::size_t slot_count);
  static Rule make_log_reduct_comp(const TermMatcher &x, const TermOpCode &op_code, std::size_t size);
  ///////******************************************* *
  static Rule make_SumVec_reduct_comp(const TermMatcher &x, TermOpCode op_code,size_t size);
  std::shared_ptr<ir::Func> func_;

  std::string name_;

  RulesByRootOp rules_by_root_op_;

  std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp_;
};

bool operator==(const Ruleset &lhs, const Ruleset &rhs);

inline bool operator!=(const Ruleset &lhs, const Ruleset &rhs)
{
  return !(lhs == rhs);
}

Ruleset operator&(const Ruleset &lhs, const Ruleset &rhs);

inline Ruleset &operator&=(Ruleset &lhs, const Ruleset &rhs)
{
  lhs = lhs & rhs;
  return lhs;
}

void print_ruleset(const Ruleset &ruleset, std::ostream &os);

std::ostream &operator<<(std::ostream &os, const Ruleset &ruleset);
} // namespace fheco::trs
