#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
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
  static Ruleset depth_after_simplify_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset depth_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset halide_adapted_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset joined_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset ops_cost_ruleset(std::shared_ptr<ir::Func> func);

  static Ruleset log2_reduct_opt_ruleset(std::shared_ptr<ir::Func> func);

  Ruleset(
    std::shared_ptr<ir::Func> func, std::string name, std::vector<Rule> add_rules, std::vector<Rule> sub_rules,
    std::vector<Rule> negate_rules, std::vector<Rule> rotate_rules, std::vector<Rule> square_rules,
    std::vector<Rule> mul_rules, std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp = nullptr)
    : func_{std::move(func)}, name_{std::move(name)}, add_rules_{std::move(add_rules)},
      sub_rules_{std::move(sub_rules)}, negate_rules_{std::move(negate_rules)}, rotate_rules_{std::move(rotate_rules)},
      square_rules_{std::move(square_rules)}, mul_rules_{std::move(mul_rules)},
      terms_ctxt_leaves_count_dp_{std::move(terms_ctxt_leaves_count_dp)}
  {}

  Ruleset(
    std::shared_ptr<ir::Func> func, std::string name, std::vector<Rule> rules,
    std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp = nullptr);

  const std::vector<Rule> &pick_rules(const ir::OpCode &op_code) const;

  inline const std::shared_ptr<ir::Func> &func() const { return func_; }

  inline std::string name() const { return name_; }

  inline const std::vector<Rule> &add_rules() const { return add_rules_; }

  inline const std::vector<Rule> &sub_rules() const { return sub_rules_; }

  inline const std::vector<Rule> &negate_rules() const { return negate_rules_; }

  inline const std::vector<Rule> &rotate_rules() const { return rotate_rules_; }

  inline const std::vector<Rule> &square_rules() const { return square_rules_; }

  inline const std::vector<Rule> &mul_rules() const { return mul_rules_; }

private:
  static std::vector<Rule> get_log_reduct_rules(
    std::size_t slot_count, const TermMatcher &x, const TermOpCode &op_code);

  static Rule make_log_reduct_comp(const TermMatcher &x, std::size_t size, const TermOpCode &op_code);

  std::shared_ptr<ir::Func> func_;

  std::string name_;

  std::vector<Rule> add_rules_;

  std::vector<Rule> sub_rules_;

  std::vector<Rule> negate_rules_;

  std::vector<Rule> rotate_rules_;

  std::vector<Rule> square_rules_;

  std::vector<Rule> mul_rules_;

  std::unique_ptr<TermsMetric> terms_ctxt_leaves_count_dp_;
};

std::ostream &operator<<(std::ostream &os, const Ruleset &ruleset);
} // namespace fheco::trs
