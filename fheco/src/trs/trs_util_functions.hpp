#pragma once
#include "ir_utils.hpp"
#include "matching_term.hpp"
#include "trs_const.hpp"
#include "trs_core.hpp"
#include <functional>
#include <unordered_map>

namespace fheco_trs
{

namespace util_functions
{
  MatchingTerm fold(
    MatchingTerm term_to_fold, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program);

  MatchingTerm get_opcode(
    MatchingTerm term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program = nullptr);

  MatchingTerm depth(
    MatchingTerm term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program);

  MatchingTerm isconst(
    const MatchingTerm &term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program);

  inline core::FunctionTable functions_table = {
    {FunctionId::fold, fold},
    {FunctionId::get_opcode, get_opcode},
    {FunctionId::depth, depth},
    {FunctionId::isconst, isconst}};
} // namespace util_functions

} // namespace fheco_trs
