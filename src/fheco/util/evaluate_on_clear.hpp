#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include <cstddef>
#include <unordered_map>

namespace fheco::util
{
ir::IOTermsInfo evaluate_on_clear(ir::Func &func, const ir::IOTermsInfo &in_terms);
} // namespace fheco::util
