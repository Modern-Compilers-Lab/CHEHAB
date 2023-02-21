#pragma once
#include "matching_term.hpp"
#include "program.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace fheco_trs
{

namespace core
{

  typedef std::function<MatchingTerm(MatchingTerm, std::unordered_map<size_t, ir::Program::Ptr> &, ir::Program *)>
    CallableFunction;

  typedef std::unordered_map<FunctionId, CallableFunction> FunctionTable;

  struct MatchingPair
  {
    MatchingTerm matching_term;
    std::shared_ptr<ir::Term> ir_node;

    MatchingPair() = default;

    MatchingPair(const MatchingTerm &m_term, const std::shared_ptr<ir::Term> &_ir_node)
      : matching_term(m_term), ir_node(_ir_node)
    {}

    MatchingPair(const MatchingPair &) = default;
    MatchingPair &operator=(const MatchingPair &) = default;

    MatchingPair(MatchingPair &&) = default;
    MatchingPair &operator=(MatchingPair &&) = default;

    ~MatchingPair() {}
  };

  std::optional<std::unordered_map<size_t, ir::Program::Ptr>> match_ir_node(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term);

  bool match_term(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term,
    std::unordered_map<size_t, ir::Program::Ptr> &matching_map);

  double arithmetic_eval(
    const MatchingTerm &term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table);

  bool evaluate_boolean_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table);

  std::vector<MatchingPair> substitute(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &rewrite_rule_rhs,
    std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program, FunctionTable &functions_table);

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map,
    std::vector<MatchingPair> &new_constants_matching_pairs, ir::Program *program, FunctionTable &functions_table);

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table);

} // namespace core

} // namespace fheco_trs
