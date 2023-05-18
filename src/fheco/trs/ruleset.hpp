#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <cstddef>
#include <memory>
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
  Ruleset(const std::shared_ptr<ir::Func> &func);

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
