#pragma once

#include "fheco/ir/func.hpp"
#include <ostream>

namespace fheco::util
{
void draw_ir(ir::Func &func, std::ostream &os);
} // namespace fheco::util
