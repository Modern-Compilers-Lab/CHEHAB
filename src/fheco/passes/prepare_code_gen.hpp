#pragma once

#include <memory>

namespace fheco::ir
{
class Func;
}

namespace fheco::passes
{
void prepare_code_gen(const std::shared_ptr<ir::Func> &func);
} // namespace fheco::passes
