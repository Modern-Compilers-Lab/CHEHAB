#pragma once
#include "program.hpp"
#include "rewrite_rule.hpp"
#include <optional>
#include <unordered_map>

namespace fheco_trs
{

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

class TRS
{
private:
  ir::Program *program;

  std::optional<std::unordered_map<size_t, std::shared_ptr<ir::Term>>> match_ir_node(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term);

  bool match_term(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term,
    std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map);

  bool check_ir_node_opcode_attribute_equality(const MatchingTerm &lhs, const MatchingTerm &rhs);

  bool evaluate_boolean_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map);

  std::vector<MatchingPair> substitute(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &rewrite_rule_rhs,
    std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map);

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map,
    std::vector<MatchingPair> &new_constants_matching_pairs);

  std::shared_ptr<ir::Term> fold_term(const std::shared_ptr<ir::Term> &const_term);

  std::vector<MatchingPair> apply_rule_on_ir_node(
    const std::shared_ptr<ir::Term> &ir_node, const RewriteRule &rule, bool &is_rule_applied);

  bool apply_rules_on_ir_node(const std::shared_ptr<ir::Term> &node, const std::vector<RewriteRule> &rules);

public:
  TRS(ir::Program *prgm) : program(prgm) {}
  void apply_rewrite_rules_on_program(const std::vector<RewriteRule> &rules);
};

} // namespace fheco_trs
