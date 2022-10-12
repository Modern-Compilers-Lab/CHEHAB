#pragma once

#include"term.hpp"
#include"program.hpp"
#include<vector>
#include<memory>

extern std::shared_ptr<ir::Program> program;

namespace fhecompiler
{

using Ptr = std::shared_ptr<ir::Term>;

template< typename T >
T operate(ir::OpCode opcode, const std::vector<Ptr>& operands, ir::TermType term_type, bool is_output=false)
{
  T new_ct(T::generate_new_label());
  program->insert_operation_node_in_dataflow(opcode, operands, new_ct.get_label(), term_type);
  return new_ct;
}

} // namespace fhecompiler
