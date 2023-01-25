#include "trs.hpp"
#include "ir_utils.hpp"
#include "trs_const.hpp"
#include <variant>

namespace fheco_trs
{

/* This function needs to handle */
bool TRS::evaluate_boolean_matching_term(
  const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{
  if (matching_term.get_opcode() == fheco_trs::OpCode::undefined)
    return true;

  if (
    matching_term.get_opcode() != fheco_trs::OpCode::undefined &&
    matching_term.get_term_type() != fheco_trs::TermType::booleanType)
    return false;

  MatchingTerm lhs = matching_term.get_operands()[0];
  MatchingTerm rhs = matching_term.get_operands()[1];

  switch (matching_term.get_opcode())
  {
  case OpCode::_and:
    return evaluate_boolean_matching_term(lhs, matching_map) && evaluate_boolean_matching_term(rhs, matching_map);
    break;

  case OpCode::_or:
    return evaluate_boolean_matching_term(lhs, matching_map) || evaluate_boolean_matching_term(rhs, matching_map);
    break;
  default:
    break;
  }

  if (rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end())
    return false;
  if (rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
    return false;

  // double
  double lhs_value, rhs_value;

  auto lhs_itr = matching_map.find(lhs.get_term_id());
  auto rhs_itr = matching_map.find(rhs.get_term_id());

  if (lhs_itr != matching_map.end())
  {
    if (lhs.get_term_type() == fheco_trs::TermType::rawDataType)
      lhs_value = std::stod(lhs_itr->second->get_label());
    else
    {
      // must be a scalar
      auto table_entry = program->get_entry_form_constants_table(lhs_itr->second->get_label());
      if (table_entry == std::nullopt)
        throw("scalar must be inserted in constatns table");

      lhs_value = ir::get_constant_value_as_double(*(*table_entry).get().get_entry_value().value);
    }
  }
  else
  {
    if (lhs.get_value() != std::nullopt)
    {
      lhs_value = ir::get_constant_value_as_double(*lhs.get_value());
    }
    else
      throw("couldnt evaluate rewrite condition");
  }

  // same steps as for lhs
  if (rhs_itr != matching_map.end())
  {
    if (rhs.get_term_type() == fheco_trs::TermType::rawDataType)
      rhs_value = std::stod(rhs_itr->second->get_label());
    else
    {
      // must be a scalar
      auto table_entry = program->get_entry_form_constants_table(rhs_itr->second->get_label());
      if (table_entry == std::nullopt)
        throw("scalar must be inserted in constatns table");

      rhs_value = ir::get_constant_value_as_double(*(*table_entry).get().get_entry_value().value);
    }
  }
  else
  {
    if (rhs.get_value() != std::nullopt)
    {
      rhs_value = ir::get_constant_value_as_double(*rhs.get_value());
    }
    else
      throw("couldnt evaluate rewrite condition");
  }

  switch (matching_term.get_opcode())
  {

  case OpCode::equal:
    return lhs_value == rhs_value;
    break;

  case OpCode::not_equal:
    return lhs_value != rhs_value;
    break;

  case OpCode::less_than:
    return lhs_value < rhs_value;
    break;

  case OpCode::less_than_or_equal:
    return lhs_value <= rhs_value;
    break;

  case OpCode::greater_than:
    return lhs_value > rhs_value;
    break;

  case OpCode::greater_than_or_equal:
    return lhs_value >= rhs_value;
    break;

  default:
    throw("unsuported operand during boolean evaluation");
    break;
  }
}

std::vector<MatchingPair> TRS::substitute(
  std::shared_ptr<ir::Term> ir_node, const MatchingTerm &rewrite_rule_rhs,
  std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{
  /*
    We call this function after ir_node is matched with lhs of rewrite rule
  */
  std::vector<MatchingPair> new_constants_matching_pairs;

  std::shared_ptr<ir::Term> new_ir_node =
    make_ir_node_from_matching_term(rewrite_rule_rhs, matching_map, new_constants_matching_pairs);

  if (new_ir_node->get_opcode() != ir::OpCode::undefined)
  {
    ir_node->clear_operands();
    ir_node->set_opcode(new_ir_node->get_opcode());
    ir_node->set_operands(new_ir_node->get_operands());
  }
  else
    ir_node->replace_with(new_ir_node);

  return new_constants_matching_pairs;
}

std::shared_ptr<ir::Term> TRS::make_ir_node_from_matching_term(
  const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map,
  std::vector<MatchingPair> &new_constants_matching_pairs)
{
  auto it = matching_map.find(matching_term.get_term_id());
  if (it != matching_map.end())
  {
    return it->second;
  }
  // create a new node
  if (matching_term.get_opcode() != fheco_trs::OpCode::undefined)
  {
    if (opcode_mapping.find(matching_term.get_opcode()) == opcode_mapping.end())
      throw("unsuported opcode in ir");

    std::vector<std::shared_ptr<ir::Term>> operands;
    for (auto &m_term_operand : matching_term.get_operands())
    {
      operands.push_back(make_ir_node_from_matching_term(m_term_operand, matching_map, new_constants_matching_pairs));
    }
    std::shared_ptr<ir::Term> ir_node =
      std::make_shared<ir::Term>(opcode_mapping[matching_term.get_opcode()], operands, "");
    ir_node->set_term_type(fheco_trs::term_type_map[matching_term.get_term_type()]);
    ir_node->set_a_default_label();

    // call for specific processes

    if (matching_term.get_fold_flag() == true)
    {
      ir_node = fold_term(ir_node);
    }

    return ir_node;
  }
  else
  {
    std::shared_ptr<ir::Term> ir_node =
      std::make_shared<ir::Term>("", fheco_trs::term_type_map[matching_term.get_term_type()]);
    ir_node->set_a_default_label();
    new_constants_matching_pairs.push_back({matching_term, ir_node});
    return ir_node;
  }
}

std::optional<std::unordered_map<size_t, std::shared_ptr<ir::Term>>> TRS::match_ir_node(
  std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term)
{
  std::unordered_map<size_t, std::shared_ptr<ir::Term>> macthing_map;
  if (match_term(ir_node, matching_term, macthing_map))
    return macthing_map;
  else
    return std::nullopt;
}

bool TRS::match_term(
  std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term,
  std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{

  // Order of if statements is important !!!

  if (ir_node->get_term_type() != fheco_trs::term_type_map[matching_term.get_term_type()])
    return false;

  auto it = matching_map.find(matching_term.get_term_id());

  if (it != matching_map.end() && it->second != ir_node)
    return false;

  if (matching_term.get_opcode() == fheco_trs::OpCode::undefined)
  {
    matching_map[matching_term.get_term_id()] = ir_node;
    return true;
  }

  if (ir_node->get_opcode() != fheco_trs::opcode_mapping[matching_term.get_opcode()])
    return false;

  bool matching_result = true;

  auto matching_term_operands = matching_term.get_operands();
  auto ir_node_operands = ir_node->get_operands();

  if (matching_term_operands.size() != ir_node_operands.size())
    return false;

  for (size_t i = 0; i < matching_term_operands.size(); i++)
  {
    matching_result = matching_result && match_term(ir_node_operands[i], matching_term_operands[i], matching_map);
    if (matching_result == false)
      break;
  }

  if (matching_result)
    matching_map[matching_term.get_term_id()] = ir_node;

  return matching_result;
}

std::shared_ptr<ir::Term> TRS::fold_term(const std::shared_ptr<ir::Term> &term)
{
  if (term->is_operation_node() == false)
    return term;

  if (term->get_term_type() == ir::ciphertextType)
    return term;

  auto operands = term->get_operands();
  if (operands.size() != 2) // only fold binary
    return term;

  // fold in depth
  std::shared_ptr<ir::Term> folded_lhs = fold_term(operands[0]);
  std::shared_ptr<ir::Term> folded_rhs = fold_term(operands[1]);

  if ((folded_lhs->get_term_type() == ir::rawDataType) && (folded_rhs->get_term_type() == ir::rawDataType))
  {
    int32_t fold_value = ir::fold_raw(folded_lhs, folded_rhs, term->get_opcode());
    std::shared_ptr<ir::Term> folded_term =
      std::make_shared<ir::Term>(ir::rawDataType); // we should take care of fold_value = 0 case
    folded_term->set_label(std::to_string(fold_value));
    return folded_term;
  }

  if (folded_rhs->get_term_type() == ir::scalarType && folded_lhs->get_term_type() == ir::scalarType)
  {
    std::shared_ptr<ir::Term> folded_term = ir::fold_scalar(folded_lhs, folded_rhs, term->get_opcode(), program);
    term->replace_with(folded_term);
    return term;
  }

  return term;

  /*
  bool is_lhs_const = program->type_of(folded_lhs->get_label()) == ir::ConstantTableEntryType::constant;
  bool is_rhs_const = program->type_of(folded_rhs->get_label()) == ir::ConstantTableEntryType::constant;

  if (is_lhs_const == false || is_rhs_const == false)
  {
    term->clear_operands();
    term->set_operands(std::vector<std::shared_ptr<ir::Term>>({folded_lhs, folded_rhs}));
    return term;
  }
  */
}

std::vector<MatchingPair> TRS::apply_rule_on_ir_node(std::shared_ptr<ir::Term> ir_node, const RewriteRule &rule)
{
  auto matching_map = match_ir_node(ir_node, rule.get_lhs());
  if (matching_map != std::nullopt)
  {
    std::cout << "matched ... \n";
    if (rule.get_rewrite_condition() != std::nullopt)
    {
      std::cout << "checking condition ...\n";
      if (evaluate_boolean_matching_term(*(rule.get_rewrite_condition()), *matching_map))
      {
        return substitute(ir_node, rule.get_rhs(), *matching_map);
      }
      else
        return {};
    }
    else
    {
      return substitute(ir_node, rule.get_rhs(), *matching_map);
    }
  }
  else
    return {};
}

void TRS::apply_trs_rewrite_rules_on_program(const std::vector<RewriteRule> &rules)
{
  auto nodes_sorted = program->get_dataflow_sorted_nodes(true);
  for (auto &node : nodes_sorted)
  {
    for (auto &rule : rules)
    {
      /*
        new_nodes_matching_pairs supposed to contain matching pairs for constants only
      */
      auto new_nodes_matching_pairs = apply_rule_on_ir_node(node, rule);
      for (auto &matching_pair : new_nodes_matching_pairs)
      {
        if (matching_pair.matching_term.get_value() == std::nullopt)
          throw("only constant are supposed to be inserted as new terms");

        ir::ConstantTableEntry c_table_entry(
          ir::ConstantTableEntryType::constant,
          {matching_pair.ir_node->get_label(), *(matching_pair.matching_term.get_value())});
        program->insert_entry_in_constants_table({matching_pair.ir_node->get_label(), c_table_entry});
      }
    }
  }
}

} // namespace fheco_trs
