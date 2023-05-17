#include "fheco/util/evaluate_on_clear.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::util
{
ir::IOTermsInfo evaluate_on_clear(const shared_ptr<ir::Func> &func, const ir::IOTermsInfo &in_terms)
{
  const auto &evaluator = func->clear_data_evaluator();
  ir::TermsValues temps_values;
  ir::IOTermsInfo outputs_values;
  for (auto term : func->get_top_sorted_terms())
  {
    if (term->is_leaf())
    {
      if (auto input_info = func->get_input_info(term->id()); input_info)
      {
        if (auto in_val_it = in_terms.find(term->id()); in_val_it != in_terms.end() && in_val_it->second.example_val_)
        {
          PackedVal in_val = *in_val_it->second.example_val_;
          evaluator.adjust_packed_val(in_val);
          if (auto output_info = func->get_output_info(term->id()); output_info)
          {
            temps_values.emplace(term->id(), in_val);
            outputs_values.emplace(term->id(), ir::ParamTermInfo{output_info->label_, move(in_val)});
          }
          else
            temps_values.emplace(term->id(), move(in_val));
        }
        else
          cerr << "value not provided for input (id=" << term->id() << ", label=" << input_info->label_ << ")\n";
      }
      else if (auto const_value = func->get_const_val(term->id()); const_value)
        temps_values.emplace(term->id(), *const_value);
      else
        throw logic_error("invalid leaf term, non-input and non-const");
    }
    else
    {
      if (term->op_code().arity() == 1)
      {
        auto arg_id = term->operands()[0]->id();
        if (auto arg_val_it = temps_values.find(arg_id); arg_val_it != temps_values.end())
        {
          switch (term->op_code().type())
          {
          case ir::OpCode::Type::encrypt:
          case ir::OpCode::Type::mod_switch:
          case ir::OpCode::Type::relin:
            temps_values.emplace(term->id(), arg_val_it->second);
            break;

          default:
            PackedVal dest_val;
            evaluator.operate_unary(term->op_code(), arg_val_it->second, dest_val);
            temps_values.emplace(term->id(), dest_val);
            break;
          }
        }
        else
          cerr << "missing arg when computing term (id=" << term->id() << ")\n";
      }
      else if (term->op_code().arity() == 2)
      {
        auto arg1_id = term->operands()[0]->id();
        if (auto arg1_val_it = temps_values.find(arg1_id); arg1_val_it != temps_values.end())
        {
          auto arg2_id = term->operands()[1]->id();
          if (auto arg2_val_it = temps_values.find(arg2_id); arg2_val_it != temps_values.end())
          {
            PackedVal dest_val;
            evaluator.operate_binary(term->op_code(), arg1_val_it->second, arg2_val_it->second, dest_val);
            temps_values.emplace(term->id(), dest_val);
          }
          else
            cerr << "missing arg when computing term (arg2 id=" << arg2_id << ")\n";
        }
        else
          cerr << "missing arg when computing term (arg1 id=" << arg1_id << ")\n";
      }
      else
        throw logic_error("unhandled clear data evaluation for operations with arity > 2");

      if (auto output_info = func->get_output_info(term->id()); output_info)
      {
        if (auto term_val_it = temps_values.find(term->id()); term_val_it != temps_values.end())
          outputs_values.emplace(term->id(), ir::ParamTermInfo{output_info->label_, term_val_it->second});
        else
          cerr << "could not compute output term (output id=" << term->id() << ")\n";
      }
    }
  }
  return outputs_values;
}
} // namespace fheco::util
