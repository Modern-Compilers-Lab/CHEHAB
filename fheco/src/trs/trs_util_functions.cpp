#include "trs_util_functions.hpp"
#include "ir_utils.hpp"

namespace fheco_trs
{
namespace util_functions
{

  MatchingTerm fold(MatchingTerm term_to_fold, core::MatchingMap &matching_map, ir::Program *program)
  {
    ir::Program::Ptr folded_ir_node = ir::fold_ir_term(matching_map[term_to_fold.get_term_id()], program);
    matching_map[term_to_fold.get_term_id()] = folded_ir_node;

    return term_to_fold;
  }

  MatchingTerm get_opcode(MatchingTerm term, core::MatchingMap &matching_map, ir::Program *program)
  {
    auto it = matching_map.find(term.get_term_id());
    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in get_opcode");

    // comparison needs to be done with ir::OpCode and not fheco_trs::OpCode

    return static_cast<int>(it->second->get_opcode());
  }

  MatchingTerm depth(MatchingTerm term, core::MatchingMap &matching_map, ir::Program *program)
  {
    auto it = matching_map.find(term.get_term_id());

    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in depth");

    auto ir_term = it->second;
    int32_t depth_of_ir_term = ir::compute_depth_of(ir_term);

    return depth_of_ir_term;
  }

  MatchingTerm isconst(const MatchingTerm &term, core::MatchingMap &matching_map, ir::Program *program)
  {

    auto it = matching_map.find(term.get_term_id());
    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in isconst");

    ir::Program::Ptr ir_node = it->second;

    if (ir_node->is_operation_node() == true)
      return false;

    return (static_cast<int>(program->type_of(ir_node->get_label()) == ir::ConstantTableEntryType::constant) == 1) ||
           (program->get_entry_value_value(ir_node->get_label()) != std::nullopt);
  }
} // namespace util_functions

} // namespace fheco_trs
