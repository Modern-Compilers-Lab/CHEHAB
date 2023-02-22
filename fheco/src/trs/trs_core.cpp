#include "trs_core.hpp"
#include "ir_utils.hpp"
#include "trs_const.hpp"

namespace fheco_trs
{
namespace core
{

  std::optional<std::unordered_map<size_t, ir::Program::Ptr>> match_ir_node(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term)
  {
    std::unordered_map<size_t, ir::Program::Ptr> matching_map;

    if (match_term(ir_node, matching_term, matching_map))
      return matching_map;

    return std::nullopt;
  }

  bool match_term(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &matching_term,
    std::unordered_map<size_t, ir::Program::Ptr> &matching_map)
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

  double arithmetic_eval(
    const MatchingTerm &term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table)
  {
    /*
      This method evaluates arithmetic expressions that involve scalarType and rawDataType
    */

    auto it = functions_table.find(term.get_function_id());

    if (it != functions_table.end())
    {
      auto next_term = it->second(term.get_operands()[0], matching_map, program);
      return arithmetic_eval(next_term, matching_map, program, functions_table);
    }

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
        {
          throw("arithmetic evaluation impossible");
        }

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

      double lhs_value = arithmetic_eval(term.get_operands()[0], matching_map, program, functions_table);
      double rhs_value = arithmetic_eval(term.get_operands()[1], matching_map, program, functions_table);

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

  bool evaluate_boolean_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table)
  {

    // there could be arithmetic expressions but at this level all needs to be evaluated

    if (matching_term.get_opcode() == OpCode::_not)
    {
      auto next_term_to_evaluate = matching_term.get_operands()[0];

      return !evaluate_boolean_matching_term(next_term_to_evaluate, matching_map, program, functions_table);
    }

    auto it = functions_table.find(matching_term.get_function_id());

    if (it != functions_table.end())
    {
      /*
        Here we don't care about calls with type as non booleanType because these concern arithmetic_eval and they are
        leaves so it will be handled below and in arithmetic_eval
      */
      if (matching_term.get_term_type() == TermType::booleanType)
      {
        if (matching_term.get_operands().size() == 0)
          throw("missing argument for function to call in evaluate_boolean_matching_term");

        auto next_term_to_evaluate =
          functions_table[matching_term.get_function_id()](matching_term.get_operands()[0], matching_map, program);

        return evaluate_boolean_matching_term(next_term_to_evaluate, matching_map, program, functions_table);
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
      return evaluate_boolean_matching_term(lhs, matching_map, program, functions_table) &&
             evaluate_boolean_matching_term(rhs, matching_map, program, functions_table);
      break;

    case OpCode::_or:
      return evaluate_boolean_matching_term(lhs, matching_map, program, functions_table) ||
             evaluate_boolean_matching_term(rhs, matching_map, program, functions_table);
      break;
    default:
      break;
    }

    if (rewrite_condition_types.find(lhs.get_term_type()) == rewrite_condition_types.end())
      return false;
    if (rewrite_condition_types.find(rhs.get_term_type()) == rewrite_condition_types.end())
      return false;

    // nodes which arithmetic operands are leaves
    double lhs_value, rhs_value;

    lhs_value = arithmetic_eval(lhs, matching_map, program, functions_table);
    rhs_value = arithmetic_eval(rhs, matching_map, program, functions_table);

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

  void substitute(
    std::shared_ptr<ir::Term> ir_node, const MatchingTerm &rewrite_rule_rhs,
    std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program, FunctionTable &functions_table)
  {
    /*
      We call this function after ir_node is matched with lhs of rewrite rule
    */
    std::shared_ptr<ir::Term> new_ir_node =
      make_ir_node_from_matching_term(rewrite_rule_rhs, matching_map, program, functions_table);
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
  }

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map,
    std::vector<MatchingPair> &new_constants_matching_pairs, ir::Program *program, FunctionTable &functions_table)
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
        operands.push_back(make_ir_node_from_matching_term(
          m_term_operand, matching_map, new_constants_matching_pairs, program, functions_table));
      }

      std::shared_ptr<ir::Term> ir_node = program->insert_operation_node_in_dataflow(
        opcode_mapping[matching_term.get_opcode()], operands, "",
        fheco_trs::term_type_map[matching_term.get_term_type()]);

      matching_map[matching_term.get_term_id()] = ir_node;

      /*
        This needs to be called here, which means once the ir_node is created, cause basically the function that will
        called it will be applied on an ir node
      */
      if (matching_term.get_function_id() != FunctionId::undefined)
      {
        if (functions_table.find(matching_term.get_function_id()) == functions_table.end())
        {
          throw("undefined function id in make_ir_node_from_matching_term");
        }

        auto new_matching_term = functions_table[matching_term.get_function_id()](matching_term, matching_map, program);

        return make_ir_node_from_matching_term(new_matching_term, matching_map, program, functions_table);
      }

      return ir_node;
    }
    else
    {

      /*
        at this stage it means that wa have a MatchingTerm with a value (a constant), and we will consider it a
        temporary variable in IR
      */

      if (matching_term.get_value() == std::nullopt)
        throw("only constant matching terms are expected at this stage");

      std::shared_ptr<ir::Term> ir_node =
        std::make_shared<ir::Term>(fheco_trs::term_type_map[matching_term.get_term_type()]);
      new_constants_matching_pairs.push_back({matching_term, ir_node});

      /*
        Insert node in dataflow graph
      */
      program->insert_created_node_in_dataflow(ir_node);

      /*
        Insert in constants table
      */
      ir::ConstantTableEntry c_table_entry(
        ir::ConstantTableEntryType::constant, {ir_node->get_label(), *(matching_term.get_value())});
      program->insert_entry_in_constants_table({ir_node->get_label(), c_table_entry});

      return ir_node;
    }
  }

  std::shared_ptr<ir::Term> make_ir_node_from_matching_term(
    const MatchingTerm &matching_term, std::unordered_map<size_t, ir::Program::Ptr> &matching_map, ir::Program *program,
    FunctionTable &functions_table)
  {
    std::vector<MatchingPair> dummy_vector;
    return make_ir_node_from_matching_term(matching_term, matching_map, dummy_vector, program, functions_table);
  }

} // namespace core

} // namespace fheco_trs
