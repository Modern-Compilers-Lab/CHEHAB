#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::util
{
void init_random(PackedVal &packed_val, integer slot_min, integer slot_max)
{
  random_device rd;
  mt19937 rng(rd());
  uniform_int_distribution<integer> uni(slot_min, slot_max);
  for (auto it = packed_val.begin(); it != packed_val.end(); ++it)
    *it = uni(rng);
}

bool is_scalar(const PackedVal &packed_val)
{
  for (size_t i = 0; i < packed_val.size() - 1; ++i)
  {
    if (packed_val[i] != packed_val[i + 1])
      return false;
  }
  return true;
}

void print_io_terms_values(const shared_ptr<ir::Func> &func, ostream &os)
{
  os << func->slot_count() << " " << func->data_flow().inputs_info().size() << " "
     << func->data_flow().outputs_info().size() << '\n';
  for (const auto &input_info : func->data_flow().inputs_info())
  {
    auto input_term = input_info.first;
    os << input_info.second.label_ << " " << (input_term->type() == ir::Term::Type::cipher) << " "
       << func->clear_data_evaluator().signedness() << " ";
    if (input_info.second.example_val_)
      os << *input_info.second.example_val_;
    else
      os << "unavailable";
    os << '\n';
  }
  for (const auto &output_info : func->data_flow().outputs_info())
  {
    auto output_term = output_info.first;
    auto is_cipher = output_term->type() == ir::Term::Type::cipher;
    if (output_info.second.example_val_)
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << *output_info.second.example_val_;
        os << '\n';
      }
    }
    else
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << "unavailable\n";
      }
    }
  }
}

void print_io_terms_values(
  const shared_ptr<ir::Func> &func, const ir::InputTermsInfo &inputs_info, const ir::OutputTermsInfo &outputs_info,
  ostream &os)
{
  os << func->slot_count() << " " << inputs_info.size() << " " << outputs_info.size() << '\n';
  for (const auto &input_info : inputs_info)
  {
    auto input_term = input_info.first;
    if (auto in_info_it = func->data_flow().inputs_info().find(input_term);
        in_info_it == func->data_flow().inputs_info().end())
      throw invalid_argument("invalid input");

    os << input_info.second.label_ << " " << (input_term->type() == ir::Term::Type::cipher) << " "
       << func->clear_data_evaluator().signedness() << " ";
    if (input_info.second.example_val_)
      os << *input_info.second.example_val_;
    else
      os << "unavailable";
    os << '\n';
  }
  for (const auto &output_info : outputs_info)
  {
    auto output_term = output_info.first;
    if (auto output_info_it = func->data_flow().outputs_info().find(output_term);
        output_info_it == func->data_flow().outputs_info().end())
      throw invalid_argument("invalid output");

    auto is_cipher = output_term->type() == ir::Term::Type::cipher;
    if (output_info.second.example_val_)
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << *output_info.second.example_val_;
        os << '\n';
      }
    }
    else
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << "unavailable\n";
      }
    }
  }
}

void print_io_terms_values(const ir::InputTermsInfo &inputs_info, const ir::OutputTermsInfo &outputs_info, ostream &os)
{
  os << inputs_info.size() << " " << outputs_info.size() << '\n';
  for (const auto &input_info : inputs_info)
  {
    auto input_term = input_info.first;
    os << input_info.second.label_ << " " << (input_term->type() == ir::Term::Type::cipher) << " ";
    if (input_info.second.example_val_)
      os << *input_info.second.example_val_;
    else
      os << "unavailable";
    os << '\n';
  }
  for (const auto &output_info : outputs_info)
  {
    auto output_term = output_info.first;
    auto is_cipher = output_term->type() == ir::Term::Type::cipher;
    if (output_info.second.example_val_)
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << *output_info.second.example_val_;
        os << '\n';
      }
    }
    else
    {
      for (const auto &label : output_info.second.labels_)
      {
        os << label << " " << is_cipher << " ";
        os << "unavailable\n";
      }
    }
  }
}

void print_const_terms_values(const ir::ConstTermsValues &const_terms_values, size_t lead_trail_size, ostream &os)
{
  for (const auto &term : const_terms_values)
  {
    os << '$' << term.first->id() << " ";
    if (term.second.is_scalar_)
      os << "scalar " << term.second.val_;
    else
      print_packed_val(term.second.val_, lead_trail_size, os);
    os << '\n';
  }
}

void print_terms_values(const ir::TermsValues &terms_values, size_t lead_trail_size, ostream &os)
{
  for (const auto &term : terms_values)
  {
    os << '$' << term.first->id() << " ";
    print_packed_val(term.second, lead_trail_size, os);
    os << '\n';
  }
}

void print_packed_val(const PackedVal &packed_val, size_t lead_trail_size, ostream &os)
{
  size_t slot_count = packed_val.size();
  if (slot_count < 2 * lead_trail_size)
    throw invalid_argument("packed_val size must at least twice lead_trail_size");

  if (slot_count == 0)
    return;

  for (size_t i = 0; i < lead_trail_size; ++i)
    os << packed_val[i] << " ";
  if (slot_count > 2 * lead_trail_size)
    os << "... ";
  for (size_t i = slot_count - lead_trail_size; i < slot_count - 1; ++i)
    os << packed_val[i] << " ";
  os << packed_val.back();
}

ostream &operator<<(ostream &os, const ir::ConstTermsValues &const_terms_values)
{
  for (const auto &term : const_terms_values)
  {
    os << '$' << term.first->id() << " ";
    if (term.second.is_scalar_)
      os << "scalar " << term.second.val_;
    else
      os << term.second.val_;
    os << '\n';
  }
  return os;
}

ostream &operator<<(ostream &os, const ir::TermsValues &terms_values)
{
  for (const auto &term : terms_values)
  {
    os << '$' << term.first->id() << " ";
    os << term.second;
    os << '\n';
  }
  return os;
}

ostream &operator<<(ostream &os, const PackedVal &packed_val)
{
  if (packed_val.empty())
    return os;

  for (auto it = packed_val.cbegin();;)
  {
    os << *it;
    ++it;
    if (it == packed_val.cend())
      break;

    os << " ";
  }
  return os;
}
} // namespace fheco::util
