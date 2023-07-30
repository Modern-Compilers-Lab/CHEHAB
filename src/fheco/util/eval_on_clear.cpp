#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/eval_on_clear.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::util
{
ir::IOValues evaluate_on_clear(const shared_ptr<ir::Func> &func, const ir::IOValues &inputs_values)
{
  const auto &evaluator = func->clear_data_evaluator();
  ir::TermsValues temps_values;
  ir::IOValues outputs_values;
  for (auto term : func->get_top_sorted_terms())
  {
    if (term->is_leaf())
    {
      if (auto input_info = func->data_flow().get_input_info(term); input_info)
      {
        if (auto input_value_it = inputs_values.find(input_info->label_); input_value_it != inputs_values.end())
        {
          PackedVal input_val = input_value_it->second;
          evaluator.adjust_packed_val(input_val);
          if (auto output_info = func->data_flow().get_output_info(term); output_info)
          {
            temps_values.emplace(term, input_val);
            for (auto it = output_info->labels_.cbegin();;)
            {
              const auto &label = *it;
              ++it;
              if (it == output_info->labels_.cend())
              {
                outputs_values.emplace(label, move(input_val));
                break;
              }
              outputs_values.emplace(label, input_val);
            }
          }
          else
            temps_values.emplace(term, move(input_val));
        }
        else
          cerr << "value not provided for input (id=" << term->id() << ", label=" << input_info->label_ << ")\n";
      }
      else if (auto const_val = func->data_flow().get_const_val(term); const_val)
        temps_values.emplace(term, *const_val);
      else
        throw logic_error("invalid leaf term, non-input and non-const");
    }
    else
    {
      if (term->op_code().arity() == 1)
      {
        auto arg = term->operands()[0];
        if (auto arg_val_it = temps_values.find(arg); arg_val_it != temps_values.end())
        {
          switch (term->op_code().type())
          {
          case ir::OpCode::Type::encrypt:
          case ir::OpCode::Type::mod_switch:
          case ir::OpCode::Type::relin:
            temps_values.emplace(term, arg_val_it->second);
            break;

          default:
            PackedVal dest_val;
            evaluator.operate_unary(term->op_code(), arg_val_it->second, dest_val);
            temps_values.emplace(term, dest_val);
            break;
          }
        }
        else
          cerr << "missing arg when computing term (id=" << term->id() << ")\n";
      }
      else if (term->op_code().arity() == 2)
      {
        auto arg1 = term->operands()[0];
        if (auto arg1_val_it = temps_values.find(arg1); arg1_val_it != temps_values.end())
        {
          auto arg2 = term->operands()[1];
          if (auto arg2_val_it = temps_values.find(arg2); arg2_val_it != temps_values.end())
          {
            PackedVal dest_val;
            evaluator.operate_binary(term->op_code(), arg1_val_it->second, arg2_val_it->second, dest_val);
            temps_values.emplace(term, dest_val);
          }
          else
            cerr << "missing arg when computing term (arg2 id=" << arg2->id() << ")\n";
        }
        else
          cerr << "missing arg when computing term (arg1 id=" << arg1->id() << ")\n";
      }
      else
        throw logic_error("unhandled clear data evaluation for operations with arity > 2");
    }
    if (auto output_info = func->data_flow().get_output_info(term); output_info)
    {
      if (auto term_val_it = temps_values.find(term); term_val_it != temps_values.end())
      {
        for (const auto &label : output_info->labels_)
          outputs_values.emplace(label, term_val_it->second);
      }
      else
        cerr << "could not compute output term (output id=" << term->id() << ")\n";
    }
  }
  return outputs_values;
}
} // namespace fheco::util
