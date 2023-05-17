#pragma once

#include "fheco/trs/term_matcher.hpp"
#include <ostream>
#include <string>
#include <utility>

namespace fheco::trs
{
class Rule
{
public:
  Rule(std::string label, TermMatcher lhs, TermMatcher rhs)
    : label_{std::move(label)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)}
  {}

  inline const std::string &label() const { return label_; };

  inline const TermMatcher &lhs() const { return lhs_; };

  inline const TermMatcher &rhs() const { return rhs_; };

private:
  std::string label_;

  TermMatcher lhs_;

  TermMatcher rhs_;
};

inline std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
  return os << rule.label();
}
} // namespace fheco::trs
