#pragma once

#include "fheco/ir/func.hpp"
#include <memory>
#include <ostream>

namespace fheco::util
{
void draw_ir(
  const std::shared_ptr<ir::Func> &func, std::ostream &os, bool show_key = false, bool impose_operands_order = false);
} // namespace fheco::util
