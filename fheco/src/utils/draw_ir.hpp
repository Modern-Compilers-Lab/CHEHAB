#pragma once

#include "program.hpp"
#include <ostream>

namespace fhecompiler
{
namespace util
{
  void draw_ir(const ir::Program &program, std::ostream &os);
} // namespace util
} // namespace fhecompiler
