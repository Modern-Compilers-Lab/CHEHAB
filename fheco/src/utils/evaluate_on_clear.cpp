#include "evaluate_on_clear.hpp"
#include <stdexcept>
#include <variant>

using namespace std;

namespace utils
{
io_variables_values evaluate_on_clear(ir::Program *program, const io_variables_values &inputs_values)
{
  const ClearDataEvaluator &evaluator = program->get_clear_data_evaluator();
  variables_values_map temps_values;
  io_variables_values outputs_values;
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
        visit(
          ir::overloaded{[&temps_values, &node, &evaluator](const auto &value) {
            temps_values.emplace(node->get_label(), init_const(evaluator, value));
          }},
          value_variant);
        break;
      }
      case ir::ConstantTableEntryType::input:
      {
        string node_tag;
        try
        {
          node_tag = program->get_tag_value_in_constants_table_entry(node->get_label());
        }
        catch (const std::string &e)
        {
          throw logic_error("input node without entry in const table (no tag)");
        }

        auto input_node_it = inputs_values.find(node_tag);
        if (input_node_it == inputs_values.end())
          throw invalid_argument("input variable value not provided");

        temps_values.emplace(node->get_label(), init_const(evaluator, input_node_it->second));
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

        int raw_arg_idx = -1;
        int raw_arg_value;

        auto arg1_value_it = temps_values.find(arg1->get_label());
        if (arg1_value_it == temps_values.end())
        {
          if (arg1->get_term_type() == ir::TermType::rawData)
          {
            raw_arg_idx = 0;
            raw_arg_value = stoi(arg1->get_label());
          }
          else
            throw logic_error("parent handled before child");
        }

        auto arg2_value_it = temps_values.find(arg2->get_label());
        if (arg2_value_it == temps_values.end())
        {
          if (arg2->get_term_type() == ir::TermType::rawData)
          {
            if (raw_arg_idx == 0)
              throw logic_error("operation with two raw_data operands");

            raw_arg_idx = 1;
            raw_arg_value = stoi(arg2->get_label());
          }
          else
            throw logic_error("parent handled before child");
        }

        if (node->get_opcode() == ir::OpCode::rotate)
        {
          variables_values_map::iterator arg_value_it;
          if (raw_arg_idx == 0)
            arg_value_it = arg2_value_it;
          else if (raw_arg_idx == 1)
            arg_value_it = arg1_value_it;
          else
            throw logic_error("rotate operation without raw_data operand (steps)");

          visit(
            ir::overloaded{
              [](const ir::ScalarValue &value) { throw logic_error("rotate a node with scalar value"); },
              [&temps_values, &node, &evaluator, raw_arg_value](const ir::VectorValue &value) {
                temps_values.emplace(node->get_label(), operate_rotate(evaluator, value, raw_arg_value));
              }},
            arg_value_it->second);
        }
        else
        {
          visit(
            ir::overloaded{[&temps_values, &node, &evaluator](const auto &arg1_value, const auto &arg2_value) {
              temps_values.emplace(
                node->get_label(), operate_binary(evaluator, node->get_opcode(), arg1_value, arg2_value));
            }},
            arg1_value_it->second, arg2_value_it->second);
        }
      }
      else if (node->get_operands().size() == 1)
      {
        const auto &arg = node->get_operands()[0];
        auto arg_value_it = temps_values.find(arg->get_label());
        if (arg_value_it == temps_values.end())
          throw logic_error("parent handled before child");

        visit(
          ir::overloaded{[&temps_values, &node, &evaluator](const auto &arg_value) {
            temps_values.emplace(node->get_label(), operate_unary(evaluator, node->get_opcode(), arg_value));
          }},
          arg_value_it->second);
      }
      else
        throw logic_error("operation node with invalid number of operands (not 1 nor 2)");

      if (program->type_of(node->get_label()) == ir::ConstantTableEntryType::output)
      {
        string node_tag;
        try
        {
          node_tag = program->get_tag_value_in_constants_table_entry(node->get_label());
        }
        catch (const std::string &e)
        {
          throw logic_error("output node without entry in const table (no tag)");
        }
        if (auto node_value = get_if<ir::VectorValue>(&temps_values[node->get_label()]))
          outputs_values.insert({node_tag, *node_value});
        else
          throw logic_error("output node having scalar value");
      }
    }
  }
  return outputs_values;
}

ir::VectorValue init_const(const ClearDataEvaluator &evaluator, const ir::VectorValue &value_var)
{
  return visit(
    ir::overloaded{[&evaluator](const auto &value) -> ir::VectorValue {
      return evaluator.make_element(value);
    }},
    value_var);
}

ir::ScalarValue init_const(const ClearDataEvaluator &evaluator, const ir::ScalarValue &value_var)
{
  return visit(
    ir::overloaded{[&evaluator](auto value) -> ir::ScalarValue {
      return value;
    }},
    value_var);
}

ir::VectorValue operate_unary(const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg)
{
  return visit(
    ir::overloaded{[op, &evaluator](const auto &arg_value) -> ir::VectorValue {
      switch (op)
      {
      case ir::OpCode::negate:
        return evaluator.negate(arg_value);
        break;
      case ir::OpCode::square:
        return evaluator.mul(arg_value, arg_value);
        break;
      case ir::OpCode::assign:
      case ir::OpCode::relinearize:
      case ir::OpCode::modswitch:
        return arg_value;
        break;
      default:
        throw logic_error("unhandled unary operation");
        break;
      }
    }},
    arg);
}

ir::ScalarValue operate_unary(const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::ScalarValue &arg)
{
  return visit(
    ir::overloaded{[op, &evaluator](const auto &arg_value) -> ir::ScalarValue {
      switch (op)
      {
      case ir::OpCode::negate:
        return evaluator.negate(arg_value);
        break;
      case ir::OpCode::square:
        return evaluator.mul(arg_value, arg_value);
        break;
      case ir::OpCode::assign:
        return arg_value;
        break;

      case ir::OpCode::relinearize:
      case ir::OpCode::modswitch:
        throw logic_error("relinearize || modswitch on node having scalar value");
        break;
      default:
        throw logic_error("unhandled unary operation");
        break;
      }
    }},
    arg);
}

ir::VectorValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg1, const ir::VectorValue &arg2)
{
  // commutative operations
  if (ir::non_commutative_ops.find(op) == ir::non_commutative_ops.end())
  {
    bool inverse_operands = arg1.index() > arg2.index();
    const auto &arg1_rectified = inverse_operands ? arg2 : arg1;
    const auto &arg2_rectified = inverse_operands ? arg1 : arg2;
    return visit(
      ir::overloaded{
        [](const vector<int64_t> &arg1_value, const vector<uint64_t> &arg2_value) -> ir::VectorValue {
          throw logic_error("could not align operands");
        },
        [op, &evaluator](const auto &arg1_value, const auto &arg2_value) -> ir::VectorValue {
          switch (op)
          {
          case ir::OpCode::add:
            return evaluator.add(arg1_value, arg2_value);
            break;
          case ir::OpCode::mul:
            return evaluator.mul(arg1_value, arg2_value);
            break;
          default:
            throw logic_error("unhandled commutative binary operation");
            break;
          }
        }},
      arg1_rectified, arg2_rectified);
  }
  else
  {
    if (arg1.index() <= arg2.index())
    {
      return visit(
        ir::overloaded{
          [](const vector<int64_t> &arg1_value, const vector<uint64_t> &arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](const auto &arg1_value, const auto &arg2_value) -> ir::VectorValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.sub(arg1_value, arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg1, arg2);
    }
    else
    {
      return visit(
        ir::overloaded{
          [](const vector<int64_t> &arg1_value, const vector<uint64_t> &arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](const auto &arg1_value, const auto &arg2_value) -> ir::VectorValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.add(evaluator.negate(arg1_value), arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg2, arg1);
    }
  }
}

ir::VectorValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::VectorValue &arg1, const ir::ScalarValue &arg2)
{
  // commutative operations
  if (ir::non_commutative_ops.find(op) == ir::non_commutative_ops.end())
  {
    if (arg1.index() <= arg2.index())
    {
      return visit(
        ir::overloaded{
          [](const vector<int64_t> &arg1_value, uint64_t arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](const auto &arg1_value, auto arg2_value) -> ir::VectorValue {
            vector<int64_t> destination_value;
            switch (op)
            {
            case ir::OpCode::add:
              return evaluator.add(arg1_value, arg2_value);
              break;
            case ir::OpCode::mul:
              return evaluator.mul(arg1_value, arg2_value);
              break;
            default:
              throw logic_error("unhandled commutative binary operation");
              break;
            }
          }},
        arg1, arg2);
    }
    else
    {
      return visit(
        ir::overloaded{
          [](int64_t arg1_value, const vector<uint64_t> &arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](auto arg1_value, const auto &arg2_value) -> ir::VectorValue {
            vector<int64_t> destination_value;
            switch (op)
            {
            case ir::OpCode::add:
              return evaluator.add(arg1_value, arg2_value);
              break;
            case ir::OpCode::mul:
              return evaluator.mul(arg1_value, arg2_value);
              break;
            default:
              throw logic_error("unhandled commutative binary operation");
              break;
            }
          }},
        arg2, arg1);
    }
  }
  else
  {
    if (arg1.index() <= arg2.index())
    {
      return visit(
        ir::overloaded{
          [](const vector<int64_t> &arg1_value, uint64_t arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](const auto &arg1_value, const auto &arg2_value) -> ir::VectorValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.sub(arg1_value, arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg1, arg2);
    }
    else
    {
      return visit(
        ir::overloaded{
          [](int64_t arg1_value, const vector<uint64_t> &arg2_value) -> ir::VectorValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](const auto &arg1_value, const auto &arg2_value) -> ir::VectorValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.add(evaluator.negate(arg1_value), arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg2, arg1);
    }
  }
}

ir::ScalarValue operate_binary(
  const ClearDataEvaluator &evaluator, ir::OpCode op, const ir::ScalarValue &arg1, const ir::ScalarValue &arg2)
{
  // commutative operations
  if (ir::non_commutative_ops.find(op) == ir::non_commutative_ops.end())
  {
    bool inverse_operands = arg1.index() > arg2.index();
    const auto &arg1_rectified = inverse_operands ? arg2 : arg1;
    const auto &arg2_rectified = inverse_operands ? arg1 : arg2;
    return visit(
      ir::overloaded{
        [](int64_t arg1_value, uint64_t arg2_value) -> ir::ScalarValue {
          throw logic_error("could not align operands");
        },
        [op, &evaluator](auto arg1_value, auto arg2_value) -> ir::ScalarValue {
          switch (op)
          {
          case ir::OpCode::add:
            return evaluator.add(arg1_value, arg2_value);
            break;
          case ir::OpCode::mul:
            return evaluator.mul(arg1_value, arg2_value);
            break;
          default:
            throw logic_error("unhandled commutative binary operation");
            break;
          }
        }},
      arg1_rectified, arg2_rectified);
  }
  else
  {
    if (arg1.index() <= arg2.index())
    {
      return visit(
        ir::overloaded{
          [](int64_t arg1_value, uint64_t arg2_value) -> ir::ScalarValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](auto arg1_value, auto arg2_value) -> ir::ScalarValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.sub(arg1_value, arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg1, arg2);
    }
    else
    {
      return visit(
        ir::overloaded{
          [](int64_t arg1_value, uint64_t arg2_value) -> ir::ScalarValue {
            throw logic_error("could not align operands");
          },
          [op, &evaluator](auto arg1_value, auto arg2_value) -> ir::ScalarValue {
            switch (op)
            {
            case ir::OpCode::sub:
              return evaluator.add(evaluator.negate(arg1_value), arg2_value);
              break;
            default:
              throw logic_error("unhandled non commutative binary operation");
              break;
            }
          }},
        arg2, arg1);
    }
  }
}

ir::VectorValue operate_rotate(const ClearDataEvaluator &evaluator, const ir::VectorValue &arg, int steps)
{
  return visit(
    ir::overloaded{[&evaluator, steps](const auto &arg_value) -> ir::VectorValue {
      return evaluator.rotate(arg_value, steps);
    }},
    arg);
}

void print_variables_values(const io_variables_values &m, size_t lead_trail_size, ostream &os)
{
  for (const auto &variable : m)
  {
    os << variable.first << " ";
    visit(
      ir::overloaded{[lead_trail_size, &os](const auto &value) {
        print_vector(value, lead_trail_size, os);
      }},
      variable.second);
    os << '\n';
  }
}
} // namespace utils

namespace std
{
ostream &operator<<(ostream &os, const utils::io_variables_values &m)
{
  for (const auto &variable : m)
  {
    os << variable.first << " ";
    visit(
      ir::overloaded{[&os](const auto &value) {
        os << value;
      }},
      variable.second);
    os << '\n';
  }
  return os;
}
} // namespace std
