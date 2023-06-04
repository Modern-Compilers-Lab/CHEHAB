#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/util/quantifier.hpp"
#include <stack>
#include <stdexcept>
#include <tuple>
#include <unordered_set>

using namespace std;

namespace fheco::util
{
void Quantifier::run_all_analysis(const param_select::EncryptionParams &params)
{
  compute_he_depth_info();
  count_terms_classes();
  compute_global_metrics(params);
}

void Quantifier::run_all_analysis()
{
  compute_he_depth_info();
  count_terms_classes();
}

void Quantifier::compute_he_depth_info()
{
  ctxt_leaves_depth_info_.clear();

  stack<pair<const ir::Term *, CtxtTermDepthInfo>> dfs;
  for (auto [output_term, output_info] : func_->data_flow().outputs_info())
  {
    if (output_term->type() == ir::Term::Type::cipher && output_term->is_source())
      dfs.push({output_term, CtxtTermDepthInfo{0, 0}});

    while (!dfs.empty())
    {
      auto [top_term, top_ctxt_info] = dfs.top();
      dfs.pop();
      if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
      {
        auto [it, inserted] = ctxt_leaves_depth_info_.emplace(top_term, CtxtTermDepthInfo{0, 0});
        int new_xdepth = max(it->second.xdepth_, top_ctxt_info.xdepth_);
        int new_depth = max(it->second.depth_, top_ctxt_info.depth_);
        it->second = CtxtTermDepthInfo{new_xdepth, new_depth};
        continue;
      }
      int operands_xdepth = top_ctxt_info.xdepth_;
      int operands_depth = top_ctxt_info.depth_ + 1;
      if (top_term->op_code().type() == ir::OpCode::Type::mul || top_term->op_code().type() == ir::OpCode::Type::square)
        ++operands_xdepth;
      for (const auto operand : top_term->operands())
      {
        if (operand->type() == ir::Term::Type::cipher)
          dfs.push({operand, CtxtTermDepthInfo{operands_xdepth, operands_depth}});
      }
    }
  }
  int first_leaf_xdepth = ctxt_leaves_depth_info_.begin()->second.xdepth_;
  int first_leaf_depth = ctxt_leaves_depth_info_.begin()->second.depth_;
  he_depth_summary_.min_xdepth_ = first_leaf_xdepth;
  he_depth_summary_.min_depth_ = first_leaf_depth;
  he_depth_summary_.avg_xdepth_ = 0;
  he_depth_summary_.avg_depth_ = 0;
  he_depth_summary_.max_xdepth_ = first_leaf_xdepth;
  he_depth_summary_.max_depth_ = first_leaf_xdepth;
  for (const auto &e : ctxt_leaves_depth_info_)
  {
    if (e.second.xdepth_ < he_depth_summary_.min_xdepth_)
      he_depth_summary_.min_xdepth_ = e.second.xdepth_;
    if (e.second.depth_ < he_depth_summary_.min_depth_)
      he_depth_summary_.min_depth_ = e.second.depth_;

    he_depth_summary_.avg_xdepth_ += e.second.xdepth_;
    he_depth_summary_.avg_depth_ += e.second.depth_;

    if (e.second.xdepth_ > he_depth_summary_.max_xdepth_)
      he_depth_summary_.max_xdepth_ = e.second.xdepth_;
    if (e.second.depth_ > he_depth_summary_.max_depth_)
      he_depth_summary_.max_depth_ = e.second.depth_;
  }
  he_depth_summary_.avg_xdepth_ /= ctxt_leaves_depth_info_.size();
  he_depth_summary_.avg_depth_ /= ctxt_leaves_depth_info_.size();

  depth_metrics_ = true;
}

void Quantifier::count_terms_classes()
{
  relin_keys_count_ = 0;
  rotation_keys_count_ = 0;
  all_terms_count_ = 0;
  captured_terms_count_ = 0;
  ptxt_leaves_count_ = 0;
  pp_ops_count_ = 0;
  ctxt_leaves_count_ = 0;
  cc_mul_count_.clear();
  he_square_count_.clear();
  encrypt_count_ = 0;
  relin_count_.clear();
  he_rotate_count_.clear();
  cp_mul_count_.clear();
  mod_switch_count_.clear();
  he_add_sub_negate_count_.clear();
  ctxt_output_terms_info_.clear();
  circuit_static_cost_ = 0;

  global_metrics_ = false;

  CtxtTermsInfo ctxt_terms_info;

  unordered_set<size_t> rotation_keys_steps{};

  for (auto term : func_->get_top_sorted_terms())
  {
    ++all_terms_count_;
    if (term->is_operation())
    {
      if (term->type() == ir::Term::Type::cipher)
      {
        if (term->op_code().type() == ir::OpCode::Type::mul)
        {
          if (term->operands()[0]->type() == term->operands()[1]->type())
          {
            const auto &arg1_info = ctxt_terms_info.find(term->operands()[0])->second;
            const auto &arg2_info = ctxt_terms_info.find(term->operands()[1])->second;
            if (arg1_info.opposite_level_ != arg2_info.opposite_level_)
              throw logic_error("he operation with operands having different levels");

            ctxt_terms_info.emplace(
              term, CtxtTermInfo{arg1_info.opposite_level_, arg1_info.size_ + arg2_info.size_ - 1});
            ++captured_terms_count_;
            auto [it, inserted] = cc_mul_count_.emplace(
              CCOpInfo{
                arg1_info.opposite_level_, min(arg1_info.size_, arg2_info.size_),
                max(arg1_info.size_, arg2_info.size_)},
              1);
            if (!inserted)
              ++it->second;
          }
          else
          {
            const auto &ctxt_arg_info = (term->operands()[0]->type() == ir::Term::Type::cipher)
                                          ? ctxt_terms_info.find(term->operands()[0])->second
                                          : ctxt_terms_info.find(term->operands()[1])->second;

            ctxt_terms_info.emplace(term, CtxtTermInfo{ctxt_arg_info.opposite_level_, ctxt_arg_info.size_});
            ++captured_terms_count_;
            auto [it, inserted] =
              cp_mul_count_.emplace(CAOpInfo{ctxt_arg_info.opposite_level_, ctxt_arg_info.size_}, 1);
            if (!inserted)
              ++it->second;
          }
        }
        else if (term->op_code().type() == ir::OpCode::Type::square)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, 2 * arg_info.size_ - 1});
          ++captured_terms_count_;
          auto [it, inserted] = he_square_count_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::encrypt)
        {
          ctxt_terms_info.emplace(term, CtxtTermInfo{0, 2});
          ++captured_terms_count_;
          ++encrypt_count_;
        }
        else if (term->op_code().type() == ir::OpCode::Type::relin)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, 2});
          relin_keys_count_ = max(relin_keys_count_, static_cast<int64_t>(arg_info.size_ - 2));
          ++captured_terms_count_;
          auto [it, inserted] = relin_count_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::rotate)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, arg_info.size_});
          rotation_keys_steps.insert(term->op_code().steps());
          ++captured_terms_count_;
          auto [it, inserted] = he_rotate_count_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::mod_switch)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_ + 1, arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] = mod_switch_count_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::add || term->op_code().type() == ir::OpCode::Type::sub)
        {
          CtxtTermInfo max_arg_info;
          if (term->operands()[0]->type() == term->operands()[1]->type())
          {
            const auto &arg1_info = ctxt_terms_info.find(term->operands()[0])->second;
            const auto &arg2_info = ctxt_terms_info.find(term->operands()[1])->second;
            if (arg1_info.opposite_level_ != arg2_info.opposite_level_)
              throw logic_error("he operation with operands having different levels");

            max_arg_info.opposite_level_ = arg1_info.opposite_level_;
            max_arg_info.size_ = max(arg1_info.size_, arg2_info.size_);
          }
          else
          {
            const auto &ctxt_arg_info = (term->operands()[0]->type() == ir::Term::Type::cipher)
                                          ? ctxt_terms_info.find(term->operands()[0])->second
                                          : ctxt_terms_info.find(term->operands()[1])->second;
            max_arg_info = ctxt_arg_info;
          }
          ctxt_terms_info.emplace(term, CtxtTermInfo{max_arg_info.opposite_level_, max_arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] =
            he_add_sub_negate_count_.emplace(CAOpInfo{max_arg_info.opposite_level_, max_arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::negate)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] = he_add_sub_negate_count_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else
          throw logic_error("unhandled he operation");
      }
      else
      {
        ++captured_terms_count_;
        ++pp_ops_count_;
      }

      circuit_static_cost_ += ir::evaluate_raw_op_code(term->op_code(), term->operands());
    }
    else
    {
      if (term->type() == ir::Term::Type::cipher)
      {
        ctxt_terms_info.emplace(term, CtxtTermInfo{0, 2});
        ++captured_terms_count_;
        ++ctxt_leaves_count_;
      }
      else
      {
        ++captured_terms_count_;
        ++ptxt_leaves_count_;
      }
    }
    if (func_->data_flow().is_output(term))
      ctxt_output_terms_info_.emplace(term, ctxt_terms_info.at(term));
  }
  rotation_keys_count_ = rotation_keys_steps.size();

  terms_classes_metrics_ = true;
}

void Quantifier::compute_global_metrics(const param_select::EncryptionParams &params)
{
  circuit_cost_ = 0;
  rotation_keys_total_size_ = 0;
  relin_keys_total_size_ = 0;
  ctxt_input_terms_total_size_ = 0;
  ctxt_input_terms_count_ = 0;
  ctxt_output_terms_total_size_ = 0;

  for (auto e : cc_mul_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level * (e.first.arg1_size_ - 1) * (e.first.arg2_size_ - 1);
    int64_t op_cost =
      coeff * ir::evaluate_raw_op_code(ir::OpCode::mul, {ir::Term::Type::cipher, ir::Term::Type::cipher});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : he_square_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level * (e.first.arg_size_ - 1) * (e.first.arg_size_ - 1);
    int64_t op_cost = coeff * ir::evaluate_raw_op_code(ir::OpCode::square, {ir::Term::Type::cipher});
    circuit_cost_ += op_cost * e.second;
  }

  circuit_cost_ += encrypt_count_ * ir::evaluate_raw_op_code(ir::OpCode::encrypt, {ir::Term::Type::plain});

  for (auto e : relin_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level * (e.first.arg_size_ - 2);
    int64_t op_cost = coeff * ir::evaluate_raw_op_code(ir::OpCode::relin, {ir::Term::Type::cipher});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : he_rotate_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level;
    int64_t op_cost = coeff * ir::evaluate_raw_op_code(ir::OpCode::rotate(0), {ir::Term::Type::cipher});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : cp_mul_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level;
    int64_t op_cost =
      coeff * ir::evaluate_raw_op_code(ir::OpCode::mul, {ir::Term::Type::cipher, ir::Term::Type::plain});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : mod_switch_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level;
    int64_t op_cost = coeff * ir::evaluate_raw_op_code(ir::OpCode::mod_switch, {ir::Term::Type::cipher});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : he_add_sub_negate_count_)
  {
    int64_t level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    int64_t coeff = level;
    int64_t op_cost =
      coeff * ir::evaluate_raw_op_code(ir::OpCode::add, {ir::Term::Type::cipher, ir::Term::Type::plain});
    circuit_cost_ += op_cost * e.second;
  }

  int64_t pkey_size = 2 * (params.coeff_mod_bit_sizes().size() + 1) * params.coeff_mod_bit_sizes().size() *
                      params.poly_modulus_degree() * 8;

  rotation_keys_total_size_ = rotation_keys_count_ * pkey_size;

  relin_keys_total_size_ = relin_keys_count_ * pkey_size;

  int64_t fresh_ctxt_size = 2 * (params.coeff_mod_bit_sizes().size() - 1) * params.poly_modulus_degree() * 8;
  for (auto input_info : func_->data_flow().inputs_info())
  {
    if (input_info.first->type() == ir::Term::Type::cipher)
    {
      ctxt_input_terms_total_size_ += fresh_ctxt_size;
      ++ctxt_input_terms_count_;
    }
  }

  for (auto output_info : func_->data_flow().outputs_info())
  {
    if (output_info.first->type() == ir::Term::Type::cipher)
    {
      auto output_ctxt_info = ctxt_output_terms_info_.at(output_info.first);
      int64_t output_size = output_ctxt_info.size_ *
                            (params.coeff_mod_bit_sizes().size() - 1 - output_ctxt_info.opposite_level_) *
                            params.poly_modulus_degree() * 8;
      ctxt_output_terms_total_size_ += output_size;
    }
  }

  global_metrics_ = true;
}

Quantifier Quantifier::operator-(const Quantifier &other) const
{
  Quantifier result = *this;
  if (depth_metrics_ && other.depth_metrics())
  {
    result.he_depth_summary_ -= other.he_depth_summary_;
    result.ctxt_leaves_depth_info_ -= other.ctxt_leaves_depth_info_;
  }
  if (terms_classes_metrics_ && other.terms_classes_metrics())
  {
    result.relin_keys_count_ -= other.relin_keys_count_;
    result.rotation_keys_count_ -= other.rotation_keys_count_;
    result.all_terms_count_ -= other.all_terms_count_;
    result.captured_terms_count_ -= other.captured_terms_count_;
    result.ptxt_leaves_count_ -= other.ptxt_leaves_count_;
    result.pp_ops_count_ -= other.pp_ops_count_;
    result.ctxt_leaves_count_ -= other.ctxt_leaves_count_;
    result.cc_mul_count_ -= other.cc_mul_count_;
    result.he_square_count_ -= other.he_square_count_;
    result.encrypt_count_ -= other.encrypt_count_;
    result.relin_count_ -= other.relin_count_;
    result.he_rotate_count_ -= other.he_rotate_count_;
    result.cp_mul_count_ -= other.cp_mul_count_;
    result.mod_switch_count_ -= other.mod_switch_count_;
    result.he_add_sub_negate_count_ -= other.he_add_sub_negate_count_;
    result.ctxt_output_terms_info_ -= other.ctxt_output_terms_info_;
    result.circuit_static_cost_ -= other.circuit_static_cost_;
  }
  if (global_metrics_ && other.global_metrics())
  {
    result.circuit_cost_ -= other.circuit_cost_;
    result.rotation_keys_total_size_ -= other.rotation_keys_total_size_;
    result.relin_keys_total_size_ -= other.relin_keys_total_size_;
    result.ctxt_input_terms_total_size_ -= other.ctxt_input_terms_total_size_;
    result.ctxt_input_terms_count_ -= other.ctxt_input_terms_count_;
    result.ctxt_output_terms_total_size_ -= other.ctxt_output_terms_total_size_;
  }
  return result;
}

Quantifier &Quantifier::operator-=(const Quantifier &other)
{
  *this = *this - other;
  return *this;
}

void Quantifier::print_info(ostream &os, bool depth_details)
{
  print_line_sep(os);
  os << "global_metrics\n";
  print_global_metrics(os);
  print_line_sep(os);
  os << "he_depth_info\n";
  print_he_depth_info(os, depth_details);
  print_line_sep(os);
  os << "terms_classes_info\n";
  print_terms_classes_info(os);
}

void Quantifier::print_he_depth_info(ostream &os, bool details) const
{
  if (!depth_metrics_)
  {
    os << "depth_metrics not computed\n";
    return;
  }

  os << "ctxt_leaves_*: (xdepth, depth)\n";
  os << "min: (" << he_depth_summary_.min_xdepth_ << ", " << he_depth_summary_.min_depth_ << ")\n";
  os << "avg: (" << he_depth_summary_.avg_xdepth_ << ", " << he_depth_summary_.avg_depth_ << ")\n";
  os << "max: (" << he_depth_summary_.max_xdepth_ << ", " << he_depth_summary_.max_depth_ << ")\n";
  if (details && ctxt_leaves_depth_info_.size())
  {
    os << "depth per ctxt term, $id: (xdepth, depth)\n";
    os << ctxt_leaves_depth_info_;
  }
}

void Quantifier::print_terms_classes_info(ostream &os) const
{
  if (!terms_classes_metrics_)
  {
    os << "terms_classes_metrics not computed\n";
    return;
  }

  os << "circuit_static_cost: " << circuit_static_cost_ << '\n';
  os << "all_terms: " << all_terms_count_ << '\n';
  os << "captured_terms: " << captured_terms_count_ << " (%"
     << static_cast<double>(captured_terms_count_) / all_terms_count_ * 100 << ")\n";
  os << "ptxt_leaves: " << ptxt_leaves_count_ << '\n';
  os << "ptxt_ptxt_ops: " << pp_ops_count_ << '\n';
  os << "relin_keys: " << relin_keys_count_ << '\n';
  os << "rotation_keys: " << rotation_keys_count_ << '\n';
  os << "ctxt_leaves: " << ctxt_leaves_count_ << '\n';

  print_line_sep(os);

  if (cc_mul_count_.size())
  {
    os << "ctxt_ctxt_mul (level, arg1_size, arg2_size): count\n" << cc_mul_count_;
    print_line_sep(os);
  }

  if (he_square_count_.size())
  {
    os << "he_square (level, arg_size): count\n" << he_square_count_;
    print_line_sep(os);
  }

  if (encrypt_count_)
  {
    os << "encrypt: " << encrypt_count_ << '\n';
    print_line_sep(os);
  }

  if (relin_count_.size())
  {
    os << "relin (level, arg_size): count\n" << relin_count_;
    print_line_sep(os);
  }

  if (he_rotate_count_.size())
  {
    os << "he_rotate (level, arg_size): count\n" << he_rotate_count_;
    print_line_sep(os);
  }

  if (cp_mul_count_.size())
  {
    os << "ctxt_ptxt_mul (level, ctxt_arg_size): count\n" << cp_mul_count_;
    print_line_sep(os);
  }

  if (mod_switch_count_.size())
  {
    os << "mod_switch (level, arg_size): count\n" << mod_switch_count_;
    print_line_sep(os);
  }

  if (he_add_sub_negate_count_.size())
  {
    os << "he_add_sub_negate (level, max_args_size): count\n" << he_add_sub_negate_count_;
    print_line_sep(os);
  }

  os << "ctxt_outputs_info, $id: (level, size)\n";
  os << ctxt_output_terms_info_;
}

void Quantifier::print_global_metrics(std::ostream &os) const
{
  if (!global_metrics_)
  {
    os << "global_metrics not computed\n";
    return;
  }

  os << "circuit_cost: " << circuit_cost_ << '\n';
  os << "rotation_keys_total_size_: " << rotation_keys_total_size_ / 1024.0 << " MB (" << rotation_keys_count_
     << " keys)\n";
  os << "relin_keys_total_size_: " << relin_keys_total_size_ / 1024.0 << " MB (" << relin_keys_count_ << " keys)\n";
  os << "ctxt_input_terms_total_size_: " << ctxt_input_terms_total_size_ / 1024.0 << " MB (" << ctxt_input_terms_count_
     << " ctxt input)\n";
  os << "ctxt_output_terms_total_size_: " << ctxt_output_terms_total_size_ / 1024.0 << " MB ("
     << ctxt_output_terms_info_.size() << " ctxt output)\n";
}

size_t Quantifier::HashCCOpInfo::operator()(const CCOpInfo &cc_op_info) const
{
  size_t h = hash<size_t>{}(cc_op_info.opposite_level_);
  ir::hash_combine(h, cc_op_info.arg1_size_);
  ir::hash_combine(h, cc_op_info.arg2_size_);
  return h;
}
bool Quantifier::EqualCCOpInfo::operator()(const CCOpInfo &lhs, const CCOpInfo &rhs) const
{
  return tuple<size_t, size_t, size_t>{lhs.opposite_level_, lhs.arg1_size_, lhs.arg2_size_} ==
         tuple<size_t, size_t, size_t>{rhs.opposite_level_, rhs.arg1_size_, rhs.arg2_size_};
}

size_t Quantifier::HashCAOpInfo::operator()(const CAOpInfo &ca_op_info) const
{
  size_t h = hash<size_t>{}(ca_op_info.opposite_level_);
  ir::hash_combine(h, ca_op_info.arg_size_);
  return h;
}
bool Quantifier::EqualCAOpInfo::operator()(const CAOpInfo &lhs, const CAOpInfo &rhs) const
{
  return pair<size_t, size_t>{lhs.opposite_level_, lhs.arg_size_} ==
         pair<size_t, size_t>{rhs.opposite_level_, rhs.arg_size_};
}
} // namespace fheco::util

namespace std
{
fheco::util::Quantifier::DepthSummary operator-(
  const fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs)
{
  auto result = lhs;
  result.min_xdepth_ -= rhs.min_xdepth_;
  result.min_depth_ -= rhs.min_depth_;
  result.avg_xdepth_ -= rhs.avg_xdepth_;
  result.avg_depth_ -= rhs.avg_depth_;
  result.max_xdepth_ -= rhs.max_xdepth_;
  result.max_depth_ -= rhs.max_depth_;
  return result;
}

fheco::util::Quantifier::DepthSummary &operator-=(
  fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CtxtTermsDepthInfo operator-(
  const fheco::util::Quantifier::CtxtTermsDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermsDepthInfo &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    auto term = e.first;
    if (auto rhs_e_it = rhs.find(term); rhs_e_it != rhs.end())
      e.second -= rhs_e_it->second;
  }
  return result;
}

fheco::util::Quantifier::CtxtTermsDepthInfo &operator-=(
  fheco::util::Quantifier::CtxtTermsDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermsDepthInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CtxtTermDepthInfo operator-(
  const fheco::util::Quantifier::CtxtTermDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermDepthInfo &rhs)
{
  auto result = lhs;
  result.depth_ -= rhs.depth_;
  result.xdepth_ -= rhs.xdepth_;
  return result;
}

fheco::util::Quantifier::CtxtTermDepthInfo &operator-=(
  fheco::util::Quantifier::CtxtTermDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermDepthInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CCOpCount operator-(
  const fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    if (auto it = rhs.find(e.first); it != rhs.end())
      e.second -= it->second;
  }
  for (const auto &e : rhs)
  {
    if (result.find(e.first) == result.end())
      result.emplace(e.first, -e.second);
  }
  return result;
}

fheco::util::Quantifier::CCOpCount &operator-=(
  fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CAOpCount operator-(
  const fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    if (auto it = rhs.find(e.first); it != rhs.end())
      e.second -= it->second;
  }
  for (const auto &e : rhs)
  {
    if (result.find(e.first) == result.end())
      result.emplace(e.first, -e.second);
  }
  return result;
}

fheco::util::Quantifier::CAOpCount &operator-=(
  fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CtxtTermsInfo operator-(
  const fheco::util::Quantifier::CtxtTermsInfo &lhs, const fheco::util::Quantifier::CtxtTermsInfo &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    auto term = e.first;
    if (auto rhs_e_it = rhs.find(term); rhs_e_it != rhs.end())
      e.second -= rhs_e_it->second;
  }
  return result;
}

fheco::util::Quantifier::CtxtTermsInfo &operator-=(
  fheco::util::Quantifier::CtxtTermsInfo &lhs, const fheco::util::Quantifier::CtxtTermsInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

fheco::util::Quantifier::CtxtTermInfo operator-(
  const fheco::util::Quantifier::CtxtTermInfo &lhs, const fheco::util::Quantifier::CtxtTermInfo &rhs)
{
  auto result = lhs;
  result.opposite_level_ -= rhs.opposite_level_;
  result.size_ -= rhs.size_;
  return result;
}

fheco::util::Quantifier::CtxtTermInfo &operator-=(
  fheco::util::Quantifier::CtxtTermInfo &lhs, const fheco::util::Quantifier::CtxtTermInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier &quantifier)
{
  print_line_sep(os);
  os << "he_depth_info\n";
  quantifier.print_he_depth_info(os, true);
  print_line_sep(os);
  os << "terms_classes_info\n";
  quantifier.print_terms_classes_info(os);
  return os;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermsDepthInfo &ctxt_terms_depth_info)
{
  for (const auto &e : ctxt_terms_depth_info)
    os << '$' << e.first->id() << ": (" << e.second << ")\n";
  return os;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermDepthInfo &ctxt_term_depth_info)
{
  return os << ctxt_term_depth_info.xdepth_ << ", " << ctxt_term_depth_info.depth_;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CCOpCount &cc_op_count)
{
  int64_t total = 0;
  for (const auto &e : cc_op_count)
  {
    total += e.second;
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.arg1_size_ << ", " << e.first.arg2_size_
       << "): " << e.second << '\n';
  }
  return os << "total: " << total << '\n';
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CAOpCount &ca_op_count)
{
  int64_t total = 0;
  for (const auto &e : ca_op_count)
  {
    total += e.second;
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.arg_size_ << "): " << e.second << '\n';
  }
  return os << "total: " << total << '\n';
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermsInfo &ctxt_terms_info)
{
  for (const auto &e : ctxt_terms_info)
    os << '$' << e.first->id() << ": (" << e.second << ")\n";
  return os;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermInfo &ctxt_term_info)
{
  return os << ctxt_term_info.opposite_level_ << ", " << ctxt_term_info.size_;
}
} // namespace std
