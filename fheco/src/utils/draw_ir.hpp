#pragma once

#include "function.hpp"
#include <ostream>

namespace fhecompiler
{
namespace util
{
  void draw_ir(ir::Function &func, std::ostream &os);
} // namespace util
} // namespace fhecompiler
