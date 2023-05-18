#pragma once

#include <memory>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::passes
{
void cse_commut(const std::shared_ptr<ir::Func> &func);
} // namespace fheco::passes
