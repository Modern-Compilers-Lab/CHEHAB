#pragma once

#include "fheco/ir/common.hpp"
#include <cstddef>
#include <memory>
#include <unordered_map>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
ir::IOValues evaluate_on_clear(const std::shared_ptr<ir::Func> &func, const ir::IOValues &inputs_values);
} // namespace fheco::util
