#include "trs_util_functions.hpp"

namespace fheco_trs
{
namespace util_functions
{
  std::shared_ptr<ir::Term> fold_ir_term(const std::shared_ptr<ir::Term> &term, ir::Program *program)
  {

    if (term->is_operation_node() == false)
      return term;

    if (term->get_term_type() == ir::ciphertextType)
      return term;

    auto operands = term->get_operands();
    if (operands.size() != 2) // only fold binary
      return term;

    // fold in depth
    std::shared_ptr<ir::Term> folded_lhs = fold_ir_term(operands[0], program);
    std::shared_ptr<ir::Term> folded_rhs = fold_ir_term(operands[1], program);

    if ((folded_lhs->get_term_type() == ir::rawDataType) && (folded_rhs->get_term_type() == ir::rawDataType))
    {
      std::shared_ptr<ir::Term> folded_term = ir::fold_raw(folded_lhs, folded_rhs, term->get_opcode());
      term->replace_with(folded_term);
      return term;
    }

    if (folded_rhs->get_term_type() == ir::scalarType && folded_lhs->get_term_type() == ir::scalarType)
    {
      std::shared_ptr<ir::Term> folded_term = ir::fold_scalar(folded_lhs, folded_rhs, term->get_opcode(), program);
      term->replace_with(folded_term);
      return term;
    }

    bool is_lhs_a_const_plain = (folded_lhs->get_term_type() == ir::plaintextType) &&
                                (program->type_of(folded_lhs->get_label()) == ir::ConstantTableEntryType::constant);

    bool is_rhs_a_const_plain = (folded_rhs->get_term_type() == ir::plaintextType) &&
                                (program->type_of(folded_rhs->get_label()) == ir::ConstantTableEntryType::constant);

    if (is_lhs_a_const_plain && is_rhs_a_const_plain)
    {
      std::shared_ptr<ir::Term> folded_term = ir::fold_const_plain(folded_lhs, folded_rhs, term->get_opcode(), program);
      term->replace_with(folded_term);
      return term;
    }

    return term;
  }

  MatchingTerm fold(
    MatchingTerm term_to_fold, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program)
  {
    ir::Program::Ptr folded_ir_node = fold_ir_term(matching_map[term_to_fold.get_term_id()], program);
    matching_map[term_to_fold.get_term_id()] = folded_ir_node;

    return term_to_fold;
  }

  MatchingTerm get_opcode(
    MatchingTerm term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program)
  {
    auto it = matching_map.find(term.get_term_id());
    if (it == matching_map.end())
      throw("term doesnt exist in matching_map in get_opcode");

    // comparison needs to be done with ir::OpCode and not fheco_trs::OpCode

    return static_cast<int>(it->second->get_opcode());
  }

} // namespace util_functions

} // namespace fheco_trs
