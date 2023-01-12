#pragma once
#include "ir_const.hpp"
#include <unordered_set>

namespace fheco_passes
{

inline std::unordered_set<ir::OpCode> rotation_intruction_set = {ir::OpCode::rotate, ir::OpCode::rotate_rows};
inline std::unordered_set<ir::OpCode> arithmetic_instruction_set = {ir::OpCode::add, ir::OpCode::mul, ir::OpCode::sub};

} // namespace fheco_passes
