#include "fheco/util/evaluate_on_clear.hpp"
#include "fheco/util/common.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco
{
namespace util
{
  ir::IOTermsInfo evaluate_on_clear(ir::Function &func, const ir::IOTermsInfo &in_terms)
  {
    const auto &evaluator = func.clear_data_evaluator();
    ir::TermsValues temps_values;
    ir::IOTermsInfo outputs_values;
    for (auto term : func.get_top_sorted_terms())
    {
      if (!term->is_operation())
      {
        if (auto input_info = func.get_input_info(term->id()); input_info)
        {
          if (auto in_val_it = in_terms.find(term->id()); in_val_it != in_terms.end() && in_val_it->second.example_val)
          {
            PackedVal in_val = *in_val_it->second.example_val;
            evaluator.adjust_packed_val(in_val);
            if (auto output_info = func.get_output_info(term->id()); output_info)
            {
              temps_values.emplace(term->id(), in_val);
              outputs_values.emplace(term->id(), ir::ParamTermInfo{output_info->label, move(in_val)});
            }
            else
              temps_values.emplace(term->id(), move(in_val));
          }
          else
            cerr << "value not provided for input (id=" << term->id() << ", label=" << input_info->label << ")\n";
        }
        else if (auto const_value = func.get_const_val(term->id()); const_value)
          temps_values.emplace(term->id(), *const_value);
        else
          throw logic_error("temp leaf term");
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
              visit(
                ir::overloaded{
                  [&temps_values, &term](const PackedVal &arg_val) { temps_values.emplace(term->id(), arg_val); },
                  [&temps_values, &term, &evaluator](ScalarVal arg_val) {
                    temps_values.emplace(term->id(), evaluator.make_packed_val(arg_val));
                  }},
                arg_val_it->second);
              break;

            case ir::OpCode::Type::mod_switch:
            case ir::OpCode::Type::relin:
              visit(
                ir::overloaded{
                  [](const auto &v) {
                    throw logic_error("mod_switch and relin operates only on ciphertext terms (PackedVal)");
                  },
                  [&temps_values, &term](const PackedVal &arg_val) {
                    temps_values.emplace(term->id(), arg_val);
                  }},
                arg_val_it->second);
              break;

            default:
              visit(
                ir::overloaded{
                  [&temps_values, &term, &evaluator](const PackedVal &arg_val) {
                    PackedVal dest_val;
                    evaluator.operate_unary(term->op_code(), arg_val, dest_val);
                    temps_values.emplace(term->id(), dest_val);
                  },
                  [&temps_values, &term, &evaluator](ScalarVal arg_val) {
                    ScalarVal dest_val;
                    evaluator.operate_unary(term->op_code(), arg_val, dest_val);
                    temps_values.emplace(term->id(), dest_val);
                  }},
                arg_val_it->second);
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
              visit(
                ir::overloaded{
                  [&temps_values, &term, &evaluator](const auto &arg1_val, const auto &arg2_val) {
                    PackedVal dest_val;
                    evaluator.operate_binary(term->op_code(), arg1_val, arg2_val, dest_val);
                    temps_values.emplace(term->id(), dest_val);
                  },
                  [&temps_values, &term, &evaluator](ScalarVal arg1_val, ScalarVal arg2_val) {
                    ScalarVal dest_val;
                    evaluator.operate_binary(term->op_code(), arg1_val, arg2_val, dest_val);
                    temps_values.emplace(term->id(), dest_val);
                  }},
                arg1_val_it->second, arg2_val_it->second);
            }
            else
              cerr << "missing arg when computing term (arg2 id=" << arg2_id << ")\n";
          }
          else
            cerr << "missing arg when computing term (arg1 id=" << arg1_id << ")\n";
        }
        else
          throw logic_error("handled n-ary operation (n not 1 nor 2)");

        if (auto output_info = func.get_output_info(term->id()); output_info)
        {
          if (auto term_val_it = temps_values.find(term->id()); term_val_it != temps_values.end())
          {
            PackedVal out_val = visit(
              ir::overloaded{
                [](const auto &val) -> PackedVal {
                  throw logic_error("an output term evaluated to other than PackedVal (ScalarVal)");
                },
                [](const PackedVal &packed_val) -> PackedVal {
                  return packed_val;
                }},
              term_val_it->second);
            outputs_values.emplace(term->id(), ir::ParamTermInfo{output_info->label, move(out_val)});
          }
          else
            cerr << "could not compute output term (output id=" << term->id() << ")\n";
        }
      }
    }
    return outputs_values;
  }

} // namespace util
} // namespace fheco
