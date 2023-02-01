#include "trs.hpp"
#include "draw_ir.hpp"
#include "ir_utils.hpp"
#include "trs_const.hpp"
#include <variant>

namespace fheco_trs
{

double TRS::arithmetic_eval(
  const MatchingTerm &term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{
  /*
    This method evaluates arithmetic expressions that involve scalarType and rawDataType
  */
  if (term.get_opcode() == OpCode::undefined)
  {
    // a constant term, it must be a MatchingTerm with a value or it matches with a scalarType or rawdataType ir node
    if (term.get_value() != std::nullopt)
    {
      return ir::get_constant_value_as_double(*term.get_value());
    }
    else
    {
      auto ir_term_itr = matching_map.find(term.get_term_id());
      if (ir_term_itr == matching_map.end())
        throw("arithmetic evaluation impossible");

      if (ir_term_itr->second->get_term_type() == ir::rawDataType)
      {
        return std::stod(ir_term_itr->second->get_label());
      }
      else if (ir_term_itr->second->get_term_type() == ir::scalarType)
      {
        ir::ConstantValue constant_value =
          *(*program->get_entry_form_constants_table(ir_term_itr->second->get_label())).get().get_entry_value().value;
        return ir::get_constant_value_as_double(constant_value);
      }
      else
        throw("arithmetic evaluation impossible");
    }
  }
  else
  {
    // for now we consider only binary operations

    if (term.get_operands().size() != 2)
      throw("only binary operations are supported at the moment in arithmetic_eval");

    double lhs_value = arithmetic_eval(term.get_operands()[0], matching_map);
    double rhs_value = arithmetic_eval(term.get_operands()[1], matching_map);

    switch (term.get_opcode())
    {
    case OpCode::add:
      return lhs_value + rhs_value;
      break;

    case OpCode::mul:
      return lhs_value * rhs_value;
      break;

    case OpCode::sub:
      return lhs_value - rhs_value;
      break;
    default:
      throw("undefined opcode in arithmetic_eval");
      break;
    }
  }
}

bool TRS::evaluate_boolean_matching_term(
  const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{

  // there could be arithmetic expressions but at this level all needs to be evaluated

  if (matching_term.get_opcode() == OpCode::_not)
  {
    return !evaluate_boolean_matching_term(matching_term.get_operands()[0], matching_map);
  }

  if (term_types_attributes.find(matching_term.get_term_type()) != term_types_attributes.end())
  {
    bool evaluation_result;
    switch (matching_term.get_term_type())
    {
    case TermType::opcodeAttribute:
    {
      auto ir_node_to_check_it = matching_map.find(matching_term.get_operands()[0].get_term_id());
      if (ir_node_to_check_it == matching_map.end())
      {
        std::cout << "exception..\n";
        throw("no matching found for ir node to check attribute value");
      }
      return ir_node_to_check_it->second->get_opcode() == opcode_mapping[matching_term.get_opcode()];
    }
    break;

    default:
      return false;
      break;
    }
  }

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

  lhs_value = arithmetic_eval(lhs, matching_map);
  rhs_value = arithmetic_eval(rhs, matching_map);

  /*
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
  */

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
  if (new_ir_node->is_operation_node())
  {
    ir_node->clear_operands();
    ir_node->set_opcode(new_ir_node->get_opcode());
    ir_node->set_operands(new_ir_node->get_operands());
  }
  else
  {
    ir_node->replace_with(new_ir_node);
  }

  return new_constants_matching_pairs;
}

std::shared_ptr<ir::Term> TRS::make_ir_node_from_matching_term(
  const MatchingTerm &matching_term, std::unordered_map<size_t, std::shared_ptr<ir::Term>> &matching_map)
{
  std::vector<MatchingPair> dummy_vector;
  return make_ir_node_from_matching_term(matching_term, matching_map, dummy_vector);
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

    if (matching_term.get_fold_flag() == true)
    {
      ir_node = fold_term(ir_node);
    }

    program->insert_created_node_in_dataflow(ir_node);

    return ir_node;
  }
  else
  {
    std::shared_ptr<ir::Term> ir_node =
      std::make_shared<ir::Term>("", fheco_trs::term_type_map[matching_term.get_term_type()]);
    ir_node->set_a_default_label();
    new_constants_matching_pairs.push_back({matching_term, ir_node});

    program->insert_created_node_in_dataflow(ir_node);

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

std::vector<MatchingPair> TRS::apply_rule_on_ir_node(
  const std::shared_ptr<ir::Term> &ir_node, const RewriteRule &rule, bool &is_rule_applied)
{
  /*
    Let's make this recursive
  */
  auto matching_map = match_ir_node(ir_node, rule.get_lhs());
  if (matching_map != std::nullopt)
  {
    std::cout << "matched ... \n";
    if (rule.get_rewrite_condition() != std::nullopt)
    {
      std::cout << "checking condition ...\n";
      if (evaluate_boolean_matching_term(*(rule.get_rewrite_condition()), *matching_map))
      {
        is_rule_applied = true;
        return substitute(ir_node, rule.get_rhs(), *matching_map);
      }
      else
      {
        return {};
      }
    }
    else
    {
      is_rule_applied = true;
      return substitute(ir_node, rule.get_rhs(), *matching_map);
    }
  }
  else
    return {};
}

bool TRS::apply_rules_on_ir_node(const std::shared_ptr<ir::Term> &node, const std::vector<RewriteRule> &rules)
{
  for (auto &rule : rules)
  {
    /*
      new_nodes_matching_pairs supposed to contain matching pairs for constants only
    */
    bool was_rule_applied = false;
    auto new_nodes_matching_pairs = apply_rule_on_ir_node(node, rule, was_rule_applied);
    if (was_rule_applied)
    {
      for (auto &matching_pair : new_nodes_matching_pairs)
      {
        if (matching_pair.matching_term.get_value() == std::nullopt)
          throw("only constant are supposed to be inserted as new terms");

        ir::ConstantTableEntry c_table_entry(
          ir::ConstantTableEntryType::constant,
          {matching_pair.ir_node->get_label(), *(matching_pair.matching_term.get_value())});
        program->insert_entry_in_constants_table({matching_pair.ir_node->get_label(), c_table_entry});
      }
      return true;
    }
  }
  return false;
}

void TRS::apply_rewrite_rules_on_program(const std::vector<RewriteRule> &rules)
{
  size_t i = 0;
  auto &sorted_nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node : sorted_nodes)
  {
    if (node->get_opcode() == ir::OpCode::undefined)
      continue;
    while (true)
    {
      if (apply_rules_on_ir_node(node, rules) == false)
        break;
    }
  }
}

} // namespace fheco_trs
