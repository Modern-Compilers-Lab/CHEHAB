#pragma once

#include "ir_const.hpp"
#include "program.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <tuple>

namespace utils
{
std::map<std::tuple<ir::OpCode, ir::TermType, ir::TermType>, std::size_t> count_node_types(ir::Program *program);

std::map<std::string, std::size_t> count_main_node_classes(ir::Program *program);

} // namespace utils
