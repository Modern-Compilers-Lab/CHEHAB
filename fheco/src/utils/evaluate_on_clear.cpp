#include "evaluate_on_clear.hpp"
#include "clear_data_evaluator.hpp"
#include <stdexcept>
#include <variant>
#include <vector>

using namespace std;

namespace utils
{

variables_values_map evaluate_on_clear(ir::Program *program, const variables_values_map &inputs_values)
{
  auto get_plaintext_value = [program](ir::ConstantValue &value_variant) -> ir::VectorValue & {
    if (auto value = get_if<1>(&value_variant))
      return *value;
    else
      throw logic_error("plaintext node with scalar value");
  };

  auto get_scalar_value = [program](ir::ConstantValue &value_variant) -> ir::ScalarValue & {
    if (auto value = get_if<0>(&value_variant))
      return *value;
    else
      throw logic_error("scalar node with vector value");
  };

  ClearDataEvaluator evaluator(program->get_vector_size(), program->get_bit_width(), program->get_signedness());

  variables_values_map temps_values;
  variables_values_map outputs_values;
  const auto &nodes = program->get_dataflow_sorted_nodes(true);
  for (const auto &node : nodes)
  {
    auto node_value_it = temps_values.find(node->get_label());
    if (node_value_it != temps_values.end())
      throw logic_error("repeated node in dataflow_sorted_nodes");

    if (!node->is_operation_node())
    {
      switch (program->type_of(node->get_label()))
      {
      case ir::ConstantTableEntryType::constant:
      {
        auto const_table_entry_opt = program->get_entry_form_constants_table(node->get_label());
        if (!const_table_entry_opt.has_value())
          throw logic_error("constant node not in constants_table");

        ir::ConstantTableEntry &const_table_entry = *const_table_entry_opt;
        auto value_opt = const_table_entry.get_entry_value().value;
        if (!value_opt.has_value())
          throw logic_error("constant node does not have a value");

        ir::ConstantValue &value_variant = *value_opt;
        if (node->get_term_type() == ir::TermType::plaintext)
        {
          ir::VectorValue &scalar_value_variant = get_plaintext_value(value_variant);
          if (auto value = get_if<vector<int64_t>>(&scalar_value_variant))
            temps_values.insert({node->get_label(), evaluator.make_element(*value)});
          else if (auto value = get_if<vector<uint64_t>>(&scalar_value_variant))
            temps_values.insert({node->get_label(), evaluator.make_element(*value)});
          else
            throw logic_error("could not get plaintext value (maybe vector<double>)");
        }
        else if (node->get_term_type() == ir::TermType::scalar)
        {
          ir::ScalarValue &scalar_value_variant = get_scalar_value(value_variant);
          if (auto value = get_if<int64_t>(&scalar_value_variant))
            temps_values.insert({node->get_label(), evaluator.make_element(*value)});
          else if (auto value = get_if<uint64_t>(&scalar_value_variant))
            temps_values.insert({node->get_label(), evaluator.make_element(*value)});
          else
            throw logic_error("could not get scalar value (maybe double)");
        }
        break;
      }
      case ir::ConstantTableEntryType::input:
      {
        auto input_node_it = inputs_values.find(node->get_label());
        if (input_node_it == inputs_values.end())
          throw invalid_argument("input variable value not provided");

        if (auto value = get_if<vector<int64_t>>(&input_node_it->second))
          temps_values.insert({node->get_label(), evaluator.make_element(*value)});
        else if (auto value = get_if<vector<uint64_t>>(&input_node_it->second))
          temps_values.insert({node->get_label(), evaluator.make_element(*value)});
        else
          throw logic_error("could not get input value");
        break;
      }
      case ir::ConstantTableEntryType::undefined:
        // throw logic_error("maybe raw_data");
        if (node->get_term_type() != ir::TermType::rawData)
          throw logic_error("leaf node without value");
        break;
      default:
        throw logic_error("leaf node not input nor const");
        break;
      }
    }
    else
    {
      if (node->get_operands().size() == 2)
      {
        const auto &arg1 = node->get_operands()[0];
        const auto &arg2 = node->get_operands()[1];

        int raw_arg;

        auto arg1_value_it = temps_values.find(arg1->get_label());
        if (arg1_value_it == temps_values.end())
        {
          if (arg1->get_term_type() == ir::TermType::rawData)
            raw_arg = stoi(arg1->get_label());
          else
            throw logic_error("parent handled before child");
        }

        auto arg2_value_it = temps_values.find(arg2->get_label());
        if (arg2_value_it == temps_values.end())
        {
          if (arg2->get_term_type() == ir::TermType::rawData)
            raw_arg = stoi(arg2->get_label());
          else
            throw logic_error("parent handled before child");
        }

        switch (node->get_opcode())
        {
        case ir::OpCode::add:
        {
          if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<int64_t> node_result;
              evaluator.add(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.add(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.add(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.add(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else
            throw logic_error("could not get child value");
          break;
        }
        case ir::OpCode::sub:
        {
          if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<int64_t> node_result;
              evaluator.sub(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.sub(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.sub(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.sub(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else
            throw logic_error("could not get child value");
          break;
        }
        case ir::OpCode::mul:
        {
          if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<int64_t> node_result;
              evaluator.multiply(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.multiply(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_it->second))
          {
            if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.multiply(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
            {
              vector<uint64_t> node_result;
              evaluator.multiply(*arg1_value, *arg2_value, node_result);
              temps_values.insert({node->get_label(), node_result});
            }
            else
              throw logic_error("could not get child value");
          }
          else
            throw logic_error("could not get child value");
          break;
        }
        case ir::OpCode::rotate:
        {
          if (arg1_value_it == temps_values.end())
          {
            if (arg2_value_it != temps_values.end())
            {
              if (auto arg2_value = get_if<vector<int64_t>>(&arg2_value_it->second))
              {
                vector<int64_t> node_result;
                evaluator.rotate(*arg2_value, raw_arg, node_result);
                temps_values.insert({node->get_label(), node_result});
              }
              else if (auto arg2_value = get_if<vector<uint64_t>>(&arg2_value_it->second))
              {
                vector<uint64_t> node_result;
                evaluator.rotate(*arg2_value, raw_arg, node_result);
                temps_values.insert({node->get_label(), node_result});
              }
              else
                throw logic_error("could not get child value");
            }
            else
              throw logic_error("rotate operation with two raw_data operands");
          }
          else if (arg2_value_it == temps_values.end())
          {
            if (arg1_value_it != temps_values.end())
            {
              if (auto arg1_value = get_if<vector<int64_t>>(&arg1_value_it->second))
              {
                vector<int64_t> node_result;
                evaluator.rotate(*arg1_value, raw_arg, node_result);
                temps_values.insert({node->get_label(), node_result});
              }
              else if (auto arg1_value = get_if<vector<uint64_t>>(&arg1_value_it->second))
              {
                vector<uint64_t> node_result;
                evaluator.rotate(*arg1_value, raw_arg, node_result);
                temps_values.insert({node->get_label(), node_result});
              }
              else
                throw logic_error("could not get child value");
            }
            else
              throw logic_error("rotate operation with two raw_data operands");
          }
          else
            throw logic_error("rotate operation without raw_data operand");
          break;
        }
        default:
          throw logic_error("unhandled binary operation");
          break;
        }
      }
      else if (node->get_operands().size() == 1)
      {
        const auto &arg = node->get_operands()[0];
        auto arg_value_it = temps_values.find(arg->get_label());
        if (arg_value_it == temps_values.end())
          throw logic_error("parent handled before child");

        if (auto arg_value = get_if<vector<int64_t>>(&arg_value_it->second))
        {
          switch (node->get_opcode())
          {
          case ir::OpCode::encrypt:
          case ir::OpCode::assign:
            temps_values.insert({node->get_label(), *arg_value});
            break;
          case ir::OpCode::negate:
          {
            vector<int64_t> node_result;
            evaluator.negate(*arg_value, node_result);
            temps_values.insert({node->get_label(), node_result});
            break;
          }
          case ir::OpCode::square:
          {
            vector<int64_t> node_result;
            evaluator.multiply(*arg_value, *arg_value, node_result);
            temps_values.insert({node->get_label(), node_result});
            break;
          }
          default:
            throw logic_error("unhandled unary operation");
            break;
          }
        }
        else if (auto arg_value = get_if<vector<uint64_t>>(&arg_value_it->second))
        {
          switch (node->get_opcode())
          {
          case ir::OpCode::encrypt:
          case ir::OpCode::assign:
            temps_values.insert({node->get_label(), *arg_value});
            break;
          case ir::OpCode::negate:
          {
            vector<uint64_t> node_result;
            evaluator.negate(*arg_value, node_result);
            temps_values.insert({node->get_label(), node_result});
            break;
          }
          case ir::OpCode::square:
          {
            vector<uint64_t> node_result;
            evaluator.multiply(*arg_value, *arg_value, node_result);
            temps_values.insert({node->get_label(), node_result});
            break;
          }
          default:
            throw logic_error("unhandled unary operation");
            break;
          }
        }
        else
          throw logic_error("could not get child value");
      }
      else
        throw logic_error("operation node with invalid number of operands (not 1 nor 2)");

      if (program->type_of(node->get_label()) == ir::ConstantTableEntryType::output)
        outputs_values.insert({node->get_label(), temps_values[node->get_label()]});
    }
  }
  return outputs_values;
}

void print_variables_values(const variables_values_map &m, size_t print_size)
{
  for (const auto &variable : m)
  {
    cout << variable.first << ": ";
    if (auto value = get_if<vector<int64_t>>(&variable.second))
      print_vector(*value, print_size);
    else if (auto value = get_if<vector<uint64_t>>(&variable.second))
      print_vector(*value, print_size);
    else
      throw logic_error("could not get variable value");
  }
}
} // namespace utils
