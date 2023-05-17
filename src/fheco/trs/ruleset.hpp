#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/rule.hpp"
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

  inline const std::vector<Rule> &add_rules() const { return add_rules_; }

  inline const std::vector<Rule> &sub_rules() const { return sub_rules_; }

  inline const std::vector<Rule> &rotate_rules() const { return rotate_rules_; }

  inline const std::vector<Rule> &mul_rules() const { return mul_rules_; }

  inline const std::vector<Rule> &misc_rules() const { return misc_rules_; }

private:
  std::vector<Rule> add_rules_;

  std::vector<Rule> sub_rules_;

  std::vector<Rule> rotate_rules_;

  std::vector<Rule> mul_rules_;

  std::vector<Rule> misc_rules_;
};
} // namespace fheco::trs
