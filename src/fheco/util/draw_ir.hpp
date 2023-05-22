#pragma once

#include <memory>
#include <ostream>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
void draw_ir(
  const std::shared_ptr<ir::Func> &func, std::ostream &os, bool id_as_label = false, bool show_key = false,
  bool impose_operands_order = false);
} // namespace fheco::util
