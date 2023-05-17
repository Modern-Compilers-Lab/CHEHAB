#pragma once

#include "fheco/ir/func.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/subst.hpp"
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace fheco::trs
{
class TRS
{
public:
  TRS(std::shared_ptr<ir::Func> func);

  void run();

  const std::vector<Rule> &pick_rules(const ir::OpCode &op_code) const;

  inline const std::vector<Rule> &add_rules() const { return add_rules_; }

  inline const std::vector<Rule> &sub_rules() const { return sub_rules_; }

  inline const std::vector<Rule> &rotate_rules() const { return rotate_rules_; }

  inline const std::vector<Rule> &mul_rules() const { return mul_rules_; }

  inline const std::vector<Rule> &misc_rules() const { return misc_rules_; }

private:
  bool match(const TermMatcher &term_matcher, ir::Term *term, Subst &subst, std::int64_t &rel_cost) const;

  ir::Term *construct_term(const TermMatcher &term_matcher, const Subst &subst, std::int64_t &rel_cost);

  std::shared_ptr<ir::Func> func_;

  std::vector<Rule> add_rules_;

  std::vector<Rule> sub_rules_;

  std::vector<Rule> rotate_rules_;

  std::vector<Rule> mul_rules_;

  std::vector<Rule> misc_rules_;
};
} // namespace fheco::trs
