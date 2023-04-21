#pragma once

#include "fheco/ir/function.hpp"
#include <ostream>

namespace fheco
{
namespace util
{
  void draw_ir(ir::Function &func, std::ostream &os);
} // namespace util
} // namespace fheco
