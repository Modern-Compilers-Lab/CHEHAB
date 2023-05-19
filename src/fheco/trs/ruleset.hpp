#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace fheco::trs
{
class Ruleset
{
public:
  static Ruleset depth_opt_ruleset(std::size_t slot_count);

  static Ruleset log2_reduct_opt_ruleset(std::size_t slot_count);

  static Ruleset ops_type_number_opt_ruleset(std::size_t slot_count);

  Ruleset(
    std::size_t slot_count, std::vector<Rule> encrypt_rules, std::vector<Rule> add_rules, std::vector<Rule> sub_rules,
    std::vector<Rule> negate_rules, std::vector<Rule> rotate_rules, std::vector<Rule> square_rules,
    std::vector<Rule> mul_rules)
    : slot_count_{slot_count}, encrypt_rules_{std::move(encrypt_rules)}, add_rules_{std::move(add_rules)},
      sub_rules_{std::move(sub_rules)}, negate_rules_{std::move(negate_rules)}, rotate_rules_{std::move(rotate_rules)},
      square_rules_{std::move(square_rules)}, mul_rules_{std::move(mul_rules)}
  {}

  const std::vector<Rule> &pick_rules(const ir::OpCode &op_code) const;

  inline const std::vector<Rule> &encrypt_rules() const { return encrypt_rules_; }

  inline const std::vector<Rule> &add_rules() const { return add_rules_; }

  inline const std::vector<Rule> &sub_rules() const { return sub_rules_; }

  inline const std::vector<Rule> &negate_rules() const { return negate_rules_; }

  inline const std::vector<Rule> &rotate_rules() const { return rotate_rules_; }

  inline const std::vector<Rule> &square_rules() const { return square_rules_; }

  inline const std::vector<Rule> &mul_rules() const { return mul_rules_; }

private:
  std::vector<Rule> get_log_reduct_rules(const TermMatcher &x, const TermOpCode &op_code) const;

  Rule make_log_reduct_comp(const TermMatcher &x, std::size_t size, const TermOpCode &op_code) const;

  std::size_t slot_count_;

  std::vector<Rule> encrypt_rules_;

  std::vector<Rule> add_rules_;

  std::vector<Rule> sub_rules_;

  std::vector<Rule> negate_rules_;

  std::vector<Rule> rotate_rules_;

  std::vector<Rule> square_rules_;

  std::vector<Rule> mul_rules_;
};
} // namespace fheco::trs
