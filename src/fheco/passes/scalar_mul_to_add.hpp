#pragma once

#include "fheco/dsl/common.hpp"
#include <memory>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::passes
{
void convert_scalar_mul_to_add(const std::shared_ptr<ir::Func> &func, integer scalar_threshold = 1 << 20);
} // namespace fheco::passes
