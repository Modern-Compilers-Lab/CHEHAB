#pragma once

#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace fheco::ir
{
class Func;
}

namespace fheco::passes
{
void insert_relin_ops(const std::shared_ptr<ir::Func> &func, std::size_t ctxt_size_threshold);

std::size_t get_ctxt_result_size(ir::OpCode::Type op_code_type, const std::vector<std::size_t> &ctxt_args_sizes);
} // namespace fheco::passes
