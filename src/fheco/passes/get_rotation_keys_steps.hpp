#pragma once

#include <memory>
#include <unordered_set>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::passes
{
std::unordered_set<int> get_rotation_keys_steps(const std::shared_ptr<ir::Func> &func);
} // namespace fheco::passes
