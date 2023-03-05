#include "trs_util_functions.hpp"
#include "ir_utils.hpp"

namespace fheco_trs
{
namespace util_functions
{

  MatchingTerm fold(MatchingTerm term_to_fold, core::MatchingMap &matching_map, ir::Program *program)
  {
    /*
      the node created from this method needs to have an ir term type since it is a leaf node, and this is crucial
      because later based on that type we can deduce type of parents
    */
    ir::Program::Ptr folded_ir_node = ir::fold_ir_term(matching_map[term_to_fold.get_term_id()], program);
    matching_map[term_to_fold.get_term_id()] = folded_ir_node;
    if (folded_ir_node->get_term_type() == ir::TermType::plaintextType)
      term_to_fold.set_term_type(fheco_trs::TermType::plaintextType);
    else if (folded_ir_node->get_term_type() == ir::TermType::scalarType)
      term_to_fold.set_term_type(fheco_trs::TermType::scalarType);
    else if (folded_ir_node->get_term_type() == ir::TermType::rawDataType)
      term_to_fold.set_term_type(fheco_trs::TermType::rawDataType);
    else
      throw("unexpected ir node type in fold");
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

  MatchingTerm isone(const MatchingTerm &term, core::MatchingMap &matching_map, ir::Program *program)
  {
    auto it = matching_map.find(term.get_term_id());
    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in iszero");

    ir::Program::Ptr ir_node = it->second;

    if (ir_node->is_operation_node() == true)
      return false;

    auto const_value_opt = program->get_entry_value_value(ir_node->get_label());
    if (const_value_opt == std::nullopt)
      return false;

    if (ir_node->get_term_type() == ir::scalarType)
    {
      ir::Number value = ir::get_constant_value_as_number(*const_value_opt);

      return (value == 1);
    }
    else if (ir_node->get_term_type() == ir::plaintextType)
    {
      std::vector<ir::Number> number_vec;
      ir::get_constant_value_as_vector_of_number(*const_value_opt, number_vec);
      return ir::is_a_vector_of_value(number_vec, 1, program->get_vector_size());
    }
    return false;
  }

  MatchingTerm iszero(const MatchingTerm &term, core::MatchingMap &matching_map, ir::Program *program)
  {
    auto it = matching_map.find(term.get_term_id());
    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in iszero");

    ir::Program::Ptr ir_node = it->second;

    if (ir_node->is_operation_node() == true)
      return false;

    auto const_value_opt = program->get_entry_value_value(ir_node->get_label());
    if (const_value_opt == std::nullopt)
      return false;

    if (ir_node->get_term_type() == ir::scalarType)
    {
      ir::Number value = ir::get_constant_value_as_number(*const_value_opt);

      return (value == 0);
    }
    else if (ir_node->get_term_type() == ir::plaintextType)
    {
      std::vector<ir::Number> number_vec;
      ir::get_constant_value_as_vector_of_number(*const_value_opt, number_vec);
      return ir::is_a_vector_of_value(number_vec, 0, program->get_vector_size());
    }
    return false;
  }
} // namespace util_functions

} // namespace fheco_trs
