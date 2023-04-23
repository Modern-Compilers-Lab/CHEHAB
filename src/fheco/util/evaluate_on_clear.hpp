#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/function.hpp"
#include <cstddef>
#include <unordered_map>

namespace fheco
{
namespace util
{
  ir::IOTermsInfo evaluate_on_clear(ir::Function &func, const ir::IOTermsInfo &in_terms);
} // namespace util
} // namespace fheco
