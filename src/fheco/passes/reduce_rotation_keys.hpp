#pragma once

#include <cstddef>
#include <memory>
#include <unordered_set>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::passes
{
std::unordered_set<int> reduce_rotation_keys(const std::shared_ptr<ir::Func> &func, std::size_t keys_threshold);
} // namespace fheco::passes
