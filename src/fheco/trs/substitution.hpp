#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <memory>
#include <ostream>
#include <unordered_map>

namespace fheco::ir
{
class Func;
class Term;
} // namespace fheco::ir

namespace fheco::trs
{
class Substitution
{
public:
  Substitution() {}

  bool insert(TermMatcher term_matcher, ir::Term *term);

  bool insert(OpGenMatcher op_gen_matcher, int val);

  ir::Term *get(const TermMatcher &term_matcher) const;

  int get(const OpGenMatcher &op_gen_matcher) const;

  inline const std::unordered_map<TermMatcher, ir::Term *> &terms_matching() const { return terms_matching_; }

  inline const std::unordered_map<OpGenMatcher, int> &op_gen_matching() const { return op_gen_matching_; }

private:
  std::unordered_map<TermMatcher, ir::Term *> terms_matching_{};

  std::unordered_map<OpGenMatcher, int> op_gen_matching_{};
};

void pprint_substitution(const std::shared_ptr<ir::Func> &func, const Substitution &subst, std::ostream &os);

std::ostream &operator<<(std::ostream &os, const Substitution &subst);
} // namespace fheco::trs
