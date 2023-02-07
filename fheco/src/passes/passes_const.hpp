#pragma once
#include "ir_const.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace fheco_passes
{

const size_t number_of_rotation_keys_threshold = 30; // This value must >= 2*log(poly_modulus_degree)

inline std::unordered_set<ir::OpCode> rotation_intruction_set = {ir::OpCode::rotate, ir::OpCode::rotate_rows};

inline std::unordered_set<ir::OpCode> arithmetic_instruction_set = {ir::OpCode::add,       ir::OpCode::mul,
                                                                    ir::OpCode::sub,       ir::OpCode::add_plain,
                                                                    ir::OpCode::mul_plain, ir::OpCode::sub_plain};

} // namespace fheco_passes
