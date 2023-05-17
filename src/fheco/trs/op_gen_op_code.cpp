#include "fheco/trs/op_gen_op_code.hpp"

using namespace std;

namespace fheco::trs
{
const OpGenOpCode OpGenOpCode::nop{Type::nop, 0, "_"};

const OpGenOpCode OpGenOpCode::add{Type::add, 2, "+"};

const OpGenOpCode OpGenOpCode::sub{Type::sub, 2, "-"};

const OpGenOpCode OpGenOpCode::negate{Type::negate, 1, "negate"};
} // namespace fheco::trs
