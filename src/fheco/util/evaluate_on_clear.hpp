#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include <cstddef>
#include <memory>
#include <unordered_map>

namespace fheco::util
{
ir::IOTermsInfo evaluate_on_clear(const std::shared_ptr<ir::Func> &func, const ir::IOTermsInfo &in_terms);
} // namespace fheco::util
