#pragma once
#include "ir_utils.hpp"
#include "matching_term.hpp"
#include "program.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace fheco_trs
{

namespace core
{

  typedef std::unordered_map<size_t, ir::Program::Ptr> MatchingMap;

  typedef std::function<MatchingTerm(MatchingTerm, MatchingMap &, ir::Program *)> CallableFunction;

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

  std::optional<MatchingMap> match_ir_node(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term, ir::Program *program);

  bool match_term(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term, MatchingMap &matching_map,
    ir::Program *program);

  ir::Number arithmetic_eval(
    const MatchingTerm &term, MatchingMap &matching_map, ir::Program *program, FunctionTable &functions_table);

  bool evaluate_boolean_matching_term(
    const MatchingTerm &matching_term, MatchingMap &matching_map, ir::Program *program, FunctionTable &functions_table);

  void substitute(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &rewrite_rule_rhs, MatchingMap &matching_map,
    ir::Program *program, FunctionTable &functions_table);

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, MatchingMap &matching_map, ir::Program *program, FunctionTable &functions_table);

  bool circuit_saving_condition(const ir::Program::Ptr &ir_node);

  bool circuit_saving_condition_rewrite_rule_checker(const MatchingTerm &term, MatchingMap &matching_map);

} // namespace core

} // namespace fheco_trs
