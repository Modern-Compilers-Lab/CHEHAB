#pragma once

#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace fheco::ir
{
class Func;

class Term;
} // namespace fheco::ir

namespace fheco::passes
{
std::size_t lazy_relin_heuristic(const std::shared_ptr<ir::Func> &func, std::size_t ctxt_size_threshold = 3);

std::size_t relin_after_ctxt_ctxt_mul(const std::shared_ptr<ir::Func> &func);

std::size_t get_ctxt_result_size(ir::OpCode::Type op_code_type, const std::vector<std::size_t> &ctxt_args_sizes);

bool is_ctxt_ctxt_mul(const ir::Term *term);
} // namespace fheco::passes
