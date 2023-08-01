#pragma once

#include <cstddef>
#include <memory>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::passes
{
void convert_scalar_mul_to_add(const std::shared_ptr<ir::Func> &func, std::size_t conv_threshold = 20);
} // namespace fheco::passes
