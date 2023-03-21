#pragma once

#include "ir_const.hpp"
#include "program.hpp"
#include <memory>
#include <string>

namespace utils
{
void draw_ir(ir::Program *program, const std::string &output_file);

} // namespace utils
