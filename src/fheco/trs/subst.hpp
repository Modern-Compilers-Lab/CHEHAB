#pragma once

#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <optional>
#include <ostream>
#include <unordered_map>

namespace fheco::trs
{
class Subst
{
public:
  Subst() {}

  bool insert(TermMatcher term_matcher, ir::Term *term);

  bool insert(OpGenMatcher op_gen_matcher, int val);

  ir::Term *get(const TermMatcher &term_matcher) const;

  std::optional<int> get(const OpGenMatcher &op_gen_matcher) const;

  inline const std::unordered_map<TermMatcher, ir::Term *> &terms_matching() const { return terms_matching_; }

  inline const std::unordered_map<OpGenMatcher, int> &op_gen_matching() const { return op_gen_matching_; }

private:
  std::unordered_map<TermMatcher, ir::Term *> terms_matching_{};

  std::unordered_map<OpGenMatcher, int> op_gen_matching_{};
};

std::ostream &operator<<(std::ostream &os, const Subst &subst);
} // namespace fheco::trs
