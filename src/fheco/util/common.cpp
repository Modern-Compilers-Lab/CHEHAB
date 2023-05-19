#include "fheco/util/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
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

void print_io_terms_values(const shared_ptr<ir::Func> &func, ostream &os)
{
  ios_base::fmtflags f(os.flags());
  os << boolalpha;

  os << func->slot_count() << " " << func->data_flow().inputs_info().size() << " "
     << func->data_flow().outputs_info().size() << '\n';
  for (const auto &input_info : func->data_flow().inputs_info())
  {
    auto input_term = input_info.first;
    os << input_info.second.label_ << " " << (input_term->type() == ir::Term::Type::cipher) << " "
       << (func->clear_data_evaluator().signedness() || func->clear_data_evaluator().delayed_reduction());
    if (input_info.second.example_val_)
      os << " " << *input_info.second.example_val_;
    os << '\n';
  }
  for (const auto &output_info : func->data_flow().outputs_info())
  {
    auto output_term = output_info.first;
    os << output_info.second.label_ << " " << (output_term->type() == ir::Term::Type::cipher) << " "
       << (func->clear_data_evaluator().signedness() || func->clear_data_evaluator().delayed_reduction());
    if (output_info.second.example_val_)
      os << " " << *output_info.second.example_val_;
    os << '\n';
  }
  os.flags(f);
}

void print_io_terms_values(
  const shared_ptr<ir::Func> &func, const ir::IOTermsInfo &inputs_info, const ir::IOTermsInfo &outputs_info,
  ostream &os)
{
  ios_base::fmtflags f(os.flags());
  os << boolalpha;

  os << func->slot_count() << " " << inputs_info.size() << " " << outputs_info.size() << '\n';
  for (const auto &input_info : inputs_info)
  {
    auto input_term = input_info.first;
    if (auto in_info_it = func->data_flow().inputs_info().find(input_term);
        in_info_it == func->data_flow().inputs_info().end())
      throw invalid_argument("invalid input");

    os << input_info.second.label_ << " " << (input_term->type() == ir::Term::Type::cipher) << " "
       << (func->clear_data_evaluator().signedness() || func->clear_data_evaluator().delayed_reduction());
    if (input_info.second.example_val_)
      os << " " << *input_info.second.example_val_;
    os << '\n';
  }
  for (const auto &output_info : outputs_info)
  {
    auto output_term = output_info.first;
    if (auto output_info_it = func->data_flow().outputs_info().find(output_info.first);
        output_info_it == func->data_flow().outputs_info().end())
      throw invalid_argument("invalid output");

    os << output_info.second.label_ << " " << (output_term->type() == ir::Term::Type::cipher) << " "
       << (func->clear_data_evaluator().signedness() || func->clear_data_evaluator().delayed_reduction());
    if (output_info.second.example_val_)
      os << " " << *output_info.second.example_val_;
    os << '\n';
  }
  os.flags(f);
}

void print_io_terms_values(const ir::IOTermsInfo &io_terms_values, size_t lead_trail_size, ostream &os)
{
  for (const auto &term : io_terms_values)
  {
    os << '$' << term.first->id() << " " << term.second.label_;
    if (term.second.example_val_)
    {
      os << " ";
      print_packed_val(*term.second.example_val_, lead_trail_size, os);
    }
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
} // namespace fheco::util

namespace std
{
ostream &operator<<(ostream &os, const fheco::ir::IOTermsInfo &io_terms_values)
{
  for (const auto &term : io_terms_values)
  {
    os << '$' << term.first->id() << " " << term.second.label_;
    if (term.second.example_val_)
      os << " " << *term.second.example_val_;
    os << '\n';
  }
  return os;
}

ostream &operator<<(ostream &os, const fheco::ir::TermsValues &terms_values)
{
  for (const auto &term : terms_values)
  {
    os << '$' << term.first->id() << " ";
    os << term.second;
    os << '\n';
  }
  return os;
}

ostream &operator<<(ostream &os, const fheco::PackedVal &packed_val)
{
  if (packed_val.size() == 0)
    return os;

  for (size_t i = 0; i < packed_val.size() - 1; ++i)
    os << packed_val[i] << " ";
  os << packed_val.back();
  return os;
}
} // namespace std
