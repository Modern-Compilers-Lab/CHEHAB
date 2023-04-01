#pragma once

#include "ir_const.hpp"
#include "program.hpp"
#include <ostream>

namespace utils
{
void draw_ir(ir::Program *program, std::ostream &os);
} // namespace utils
