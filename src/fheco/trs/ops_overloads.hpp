#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <cstdint>
#include <vector>

namespace fheco::trs
{
// addition
TermMatcher operator+(TermMatcher lhs, TermMatcher rhs);

OpGenMatcher operator+(OpGenMatcher lhs, OpGenMatcher rhs);

// subtraction
TermMatcher operator-(TermMatcher lhs, TermMatcher rhs);

OpGenMatcher operator-(OpGenMatcher lhs, OpGenMatcher rhs);

// multiplication
TermMatcher operator*(TermMatcher lhs, TermMatcher rhs);

// negation
TermMatcher operator-(TermMatcher arg);

OpGenMatcher operator-(OpGenMatcher arg);

// rotation
TermMatcher operator<<(TermMatcher arg, OpGenMatcher steps);

TermMatcher operator>>(TermMatcher arg, OpGenMatcher steps);

// encryption
TermMatcher encrypt(TermMatcher arg);

// square
TermMatcher square(TermMatcher arg);

// mod
OpGenMatcher operator%(OpGenMatcher lhs, OpGenMatcher rhs);

TermMatcher balanced_op(const std::vector<TermMatcher> &args, const TermOpCode &op_code);
} // namespace fheco::trs
