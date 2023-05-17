#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/subst.hpp"
#include <vector>

namespace fheco::trs
{
int fold_op_gen_matcher(const OpGenMatcher &op_gen_matcher, const Subst &subst);

void operate(const OpGenOpCode &op_code, const std::vector<int> &args, int &dest);

void operate_unary(const OpGenOpCode &op_code, int arg, int &dest);

void operate_binary(const OpGenOpCode &op_code, int arg1, int arg2, int &dest);
} // namespace fheco::trs
