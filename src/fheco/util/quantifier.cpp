#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/quantifier.hpp"
#include <limits>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>

using namespace std;

namespace fheco::util
{
void print_line_sep(ostream &os);

void Quantifier::run_all_analysis(const param_select::EncParams &params)
{
  compute_depth_info();
  count_terms_classes();
  compute_global_metrics(params);
}

void Quantifier::run_all_analysis()
{
  compute_depth_info();
  count_terms_classes();
}

void Quantifier::compute_depth_info()
{
  ctxt_leaves_depth_info_.clear();

  struct Call
  {
    const ir::Term *term_;
    DepthInfo depth_info_;
  };
  stack<Call> call_stack;

  struct CompareDepthInfo
  {
    bool operator()(const DepthInfo &lhs, const DepthInfo &rhs) const
    {
      return lhs.depth_ + lhs.xdepth_ > rhs.depth_ + rhs.xdepth_;
    }
  };

  using DepthInfoSet = set<DepthInfo, CompareDepthInfo>;
  unordered_map<const ir::Term *, DepthInfoSet> emitted_calls;

  for (auto [output_term, output_info] : func_->data_flow().outputs_info())
  {
    if (output_term->type() == ir::Term::Type::cipher)
      call_stack.push({output_term, DepthInfo{0, 0}});
  }
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term = top_call.term_;
    auto top_depth_info = top_call.depth_info_;
    if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
    {
      auto [it, inserted] =
        ctxt_leaves_depth_info_.emplace(top_term, DepthInfo{top_depth_info.depth_, top_depth_info.xdepth_});
      if (!inserted)
      {
        it->second.depth_ = max(it->second.depth_, top_depth_info.depth_);
        it->second.xdepth_ = max(it->second.xdepth_, top_depth_info.xdepth_);
      }
      continue;
    }
    auto operands_depth = top_depth_info.depth_;
    if (
      top_term->op_code().type() != ir::OpCode::Type::mod_switch &&
      top_term->op_code().type() != ir::OpCode::Type::relin)
      ++operands_depth;
    auto operands_xdepth = top_depth_info.xdepth_;
    if (top_term->op_code().type() == ir::OpCode::Type::mul || top_term->op_code().type() == ir::OpCode::Type::square)
      ++operands_xdepth;

    DepthInfo operands_depth_info{operands_depth, operands_xdepth};
    for (auto operand : top_term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
      {
        if (auto it = emitted_calls.find(operand); it != emitted_calls.end())
        {
          for (const auto &depth_info : it->second)
          {
            if (operands_depth > depth_info.depth_ || operands_xdepth > depth_info.xdepth_)
            {
              it->second.insert(operands_depth_info);
              call_stack.push({operand, move(operands_depth_info)});
              break;
            }
          }
        }
        else
        {
          emitted_calls.emplace(operand, DepthInfoSet{operands_depth_info});
          call_stack.push({operand, move(operands_depth_info)});
        }
      }
    }
  }
  if (ctxt_leaves_depth_info_.empty())
    return;

  auto first_leaf_depth = ctxt_leaves_depth_info_.begin()->second.depth_;
  auto first_leaf_xdepth = ctxt_leaves_depth_info_.begin()->second.xdepth_;
  depth_summary_.min_depth_ = first_leaf_depth;
  depth_summary_.min_xdepth_ = first_leaf_xdepth;
  depth_summary_.avg_depth_ = 0;
  depth_summary_.avg_xdepth_ = 0;
  depth_summary_.max_depth_ = first_leaf_xdepth;
  depth_summary_.max_xdepth_ = first_leaf_xdepth;
  for (const auto &e : ctxt_leaves_depth_info_)
  {
    if (e.second.depth_ < depth_summary_.min_depth_)
      depth_summary_.min_depth_ = e.second.depth_;
    if (e.second.xdepth_ < depth_summary_.min_xdepth_)
      depth_summary_.min_xdepth_ = e.second.xdepth_;

    depth_summary_.avg_depth_ += e.second.depth_;
    depth_summary_.avg_xdepth_ += e.second.xdepth_;

    if (e.second.depth_ > depth_summary_.max_depth_)
      depth_summary_.max_depth_ = e.second.depth_;
    if (e.second.xdepth_ > depth_summary_.max_xdepth_)
      depth_summary_.max_xdepth_ = e.second.xdepth_;
  }
  depth_summary_.avg_depth_ /= ctxt_leaves_depth_info_.size();
  depth_summary_.avg_xdepth_ /= ctxt_leaves_depth_info_.size();

  depth_metrics_ = true;
}

void Quantifier::count_terms_classes()
{
  terms_count_ = 0;
  captured_terms_count_ = 0;
  ctxt_inputs_count_ = 0;
  ptxt_leaves_count_ = 0;
  constants_count_ = 0;
  pp_ops_count_ = 0;
  constants_ops_count_ = 0;
  circuit_static_cost_ = 0;
  cc_mul_counts_.clear();
  cc_mul_total_ = 0;
  square_counts_.clear();
  square_total_ = 0;
  encrypt_count_ = 0;
  relin_counts_.clear();
  relin_total_ = 0;
  rotate_counts_.clear();
  rotate_total_ = 0;
  cp_mul_counts_.clear();
  cp_mul_total_ = 0;
  c_scalar_mul_counts_.clear();
  c_scalar_mul_total_ = 0;
  c_non_scalar_mul_counts_.clear();
  c_non_scalar_mul_total_ = 0;
  mod_switch_counts_.clear();
  mod_switch_total_ = 0;
  he_add_counts_.clear();
  he_add_total_ = 0;
  ctxt_outputs_info_.clear();
  relin_keys_count_ = 0;
  rotation_keys_count_ = 0;

  global_metrics_ = false;

  CtxtTermsInfo ctxt_terms_info;

  unordered_set<size_t> rotation_keys_steps{};

  for (auto term : func_->get_top_sorted_terms())
  {
    ++terms_count_;
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
            ++cc_mul_total_;
            auto [it, inserted] = cc_mul_counts_.emplace(
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
            auto op_info = CAOpInfo{ctxt_arg_info.opposite_level_, ctxt_arg_info.size_};
            ++captured_terms_count_;
            ++cp_mul_total_;
            auto [it, inserted] = cp_mul_counts_.emplace(op_info, 1);
            if (!inserted)
              ++it->second;
            auto ptxt_operand =
              (term->operands()[0]->type() == ir::Term::Type::plain) ? term->operands()[0] : term->operands()[1];
            bool is_c_scalar_mul = false;
            if (auto const_info = func_->data_flow().get_const_info(ptxt_operand); const_info)
            {
              if (const_info->is_scalar_)
              {
                is_c_scalar_mul = true;
                ++c_scalar_mul_total_;
                auto [it, inserted] = c_scalar_mul_counts_.emplace(move(op_info), 1);
                if (!inserted)
                  ++it->second;
              }
            }
            if (!is_c_scalar_mul)
            {
              ++c_non_scalar_mul_total_;
              auto [it, inserted] = c_non_scalar_mul_counts_.emplace(move(op_info), 1);
              if (!inserted)
                ++it->second;
            }
          }
        }
        else if (term->op_code().type() == ir::OpCode::Type::square)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, 2 * arg_info.size_ - 1});
          ++captured_terms_count_;
          ++square_total_;
          auto [it, inserted] = square_counts_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
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
          relin_keys_count_ = max(relin_keys_count_, arg_info.size_ - 2);
          ++captured_terms_count_;
          ++relin_total_;
          auto [it, inserted] = relin_counts_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::rotate)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, arg_info.size_});
          rotation_keys_steps.insert(term->op_code().steps());
          ++captured_terms_count_;
          ++rotate_total_;
          auto [it, inserted] = rotate_counts_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::mod_switch)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_ + 1, arg_info.size_});
          ++captured_terms_count_;
          ++mod_switch_total_;
          auto [it, inserted] = mod_switch_counts_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
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
          ++he_add_total_;
          auto [it, inserted] = he_add_counts_.emplace(CAOpInfo{max_arg_info.opposite_level_, max_arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::negate)
        {
          const auto &arg_info = ctxt_terms_info.find(term->operands()[0])->second;
          ctxt_terms_info.emplace(term, CtxtTermInfo{arg_info.opposite_level_, arg_info.size_});
          ++captured_terms_count_;
          ++he_add_total_;
          auto [it, inserted] = he_add_counts_.emplace(CAOpInfo{arg_info.opposite_level_, arg_info.size_}, 1);
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
        if (func_->can_fold(term->operands()))
          ++constants_ops_count_;
      }

      circuit_static_cost_ += ir::static_eval_op(func_, term->op_code(), term->operands());
    }
    else
    {
      if (term->type() == ir::Term::Type::cipher)
      {
        ctxt_terms_info.emplace(term, CtxtTermInfo{0, 2});
        ++captured_terms_count_;
        ++ctxt_inputs_count_;
      }
      else
      {
        ++captured_terms_count_;
        ++ptxt_leaves_count_;
        if (func_->data_flow().is_const(term))
          ++constants_count_;
      }
    }
    if (func_->data_flow().is_output(term))
      ctxt_outputs_info_.emplace(term, ctxt_terms_info.at(term));
  }
  rotation_keys_count_ = rotation_keys_steps.size();

  terms_classes_metrics_ = true;
}

void Quantifier::compute_global_metrics(const param_select::EncParams &params)
{
  circuit_cost_ = 0;
  rotation_keys_size_ = 0;
  relin_keys_size_ = 0;
  ctxt_inputs_size_ = 0;
  ctxt_outputs_size_ = 0;

  circuit_cost_ += constants_count_ * ir::static_eval_op(ir::OpCode::nop, {{ir::Term::Type::plain, true, false}});

  for (auto e : cc_mul_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level * (e.first.arg1_size_ - 1) * (e.first.arg2_size_ - 1);
    auto op_cost =
      coeff * ir::static_eval_op(
                ir::OpCode::mul, {{ir::Term::Type::cipher, false, false}, {ir::Term::Type::cipher, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : square_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level * (e.first.arg_size_ - 1) * (e.first.arg_size_ - 1);
    auto op_cost = coeff * ir::static_eval_op(ir::OpCode::square, {{ir::Term::Type::cipher, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  circuit_cost_ += encrypt_count_ * ir::static_eval_op(ir::OpCode::encrypt, {{ir::Term::Type::plain, false, false}});

  for (auto e : relin_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level * (e.first.arg_size_ - 2);
    auto op_cost = coeff * ir::static_eval_op(ir::OpCode::relin, {{ir::Term::Type::cipher, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : rotate_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level;
    auto op_cost = coeff * ir::static_eval_op(ir::OpCode::rotate(0), {{ir::Term::Type::cipher, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : c_scalar_mul_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level;
    auto op_cost =
      coeff * ir::static_eval_op(
                ir::OpCode::mul, {{ir::Term::Type::cipher, false, false}, {ir::Term::Type::plain, true, true}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : c_non_scalar_mul_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level;
    auto op_cost =
      coeff * ir::static_eval_op(
                ir::OpCode::mul, {{ir::Term::Type::cipher, false, false}, {ir::Term::Type::plain, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : mod_switch_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level;
    auto op_cost = coeff * ir::static_eval_op(ir::OpCode::mod_switch, {{ir::Term::Type::cipher, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  for (auto e : he_add_counts_)
  {
    auto level = params.coeff_mod_bit_sizes().size() - e.first.opposite_level_;
    auto coeff = level;
    auto op_cost =
      coeff * ir::static_eval_op(
                ir::OpCode::add, {{ir::Term::Type::cipher, false, false}, {ir::Term::Type::plain, false, false}});
    circuit_cost_ += op_cost * e.second;
  }

  auto pkey_size =
    2 * (params.coeff_mod_bit_sizes().size() + 1) * params.coeff_mod_bit_sizes().size() * params.poly_mod_degree() * 8;

  rotation_keys_size_ = rotation_keys_count_ * pkey_size;

  relin_keys_size_ = relin_keys_count_ * pkey_size;

  auto fresh_ctxt_size = 2 * (params.coeff_mod_bit_sizes().size() - 1) * params.poly_mod_degree() * 8;
  for (auto input_info : func_->data_flow().inputs_info())
  {
    if (input_info.first->type() == ir::Term::Type::cipher)
      ctxt_inputs_size_ += fresh_ctxt_size;
  }

  for (auto output_info : func_->data_flow().outputs_info())
  {
    if (output_info.first->type() == ir::Term::Type::cipher)
    {
      auto output_ctxt_info = ctxt_outputs_info_.at(output_info.first);
      auto output_size = output_ctxt_info.size_ *
                         (params.coeff_mod_bit_sizes().size() - 1 - output_ctxt_info.opposite_level_) *
                         params.poly_mod_degree() * 8;
      ctxt_outputs_size_ += output_size;
    }
  }

  global_metrics_ = true;
}

void Quantifier::print_info(ostream &os, bool depth_details, bool outputs_details)
{
  print_line_sep(os);
  os << "global_metrics\n";
  print_global_metrics(os);
  print_line_sep(os);
  os << "depth_metrics\n";
  print_depth_info(os, depth_details);
  print_line_sep(os);
  os << "terms_classes_metrics\n";
  print_terms_classes_info(os, outputs_details);
}

void Quantifier::print_depth_info(ostream &os, bool details) const
{
  if (!depth_metrics_)
  {
    os << "depth_metrics not computed\n";
    return;
  }

  os << "ctxt_leaves_*: (depth, xdepth)\n";
  os << "max: (" << depth_summary_.max_depth_ << ", " << depth_summary_.max_xdepth_ << ")\n";
  os << "avg: (" << depth_summary_.avg_depth_ << ", " << depth_summary_.avg_xdepth_ << ")\n";
  os << "min: (" << depth_summary_.min_depth_ << ", " << depth_summary_.min_xdepth_ << ")\n";

  if (details && ctxt_leaves_depth_info_.size())
  {
    os << "depth per ctxt leaf term, $id: (depth, xdepth)\n";
    os << ctxt_leaves_depth_info_;
  }
}

void Quantifier::print_terms_classes_info(ostream &os, bool outputs_details) const
{
  if (!terms_classes_metrics_)
  {
    os << "terms_classes_metrics not computed\n";
    return;
  }

  os << "|terms|: " << terms_count_ << '\n';
  os << "captured_terms: " << captured_terms_count_ << " ("
     << static_cast<double>(captured_terms_count_) / terms_count_ * 100 << "%)\n";
  os << "|ctxt_inputs|: " << ctxt_inputs_count_ << '\n';
  os << "|ptxt_leaves|: " << ptxt_leaves_count_ << '\n';
  os << "|constants|: " << constants_count_ << '\n';
  os << "|ptxt_ptxt_ops|: " << pp_ops_count_ << '\n';
  os << "|constants_ops|: " << constants_ops_count_ << '\n';
  os << "operations_static_cost: " << circuit_static_cost_ << '\n';
  print_line_sep(os);
  os << "|mul| (level, arg1_size, arg2_size): count\n" << cc_mul_counts_;
  os << "total: " << cc_mul_total_ << '\n';
  print_line_sep(os);
  os << "|square| (level, arg_size): count\n" << square_counts_;
  os << "total: " << square_total_ << '\n';
  print_line_sep(os);
  os << "|encrypt|: " << encrypt_count_ << '\n';
  print_line_sep(os);
  os << "|relin| (level, arg_size): count\n" << relin_counts_;
  os << "total: " << relin_total_ << '\n';
  print_line_sep(os);
  os << "|rotate| (level, arg_size): count\n" << rotate_counts_;
  os << "total: " << rotate_total_ << '\n';
  print_line_sep(os);
  os << "|mul_plain| (level, ctxt_arg_size): count\n" << cp_mul_counts_;
  os << "total: " << cp_mul_total_ << '\n';
  print_line_sep(os);
  os << "|mul_scalar| (level, ctxt_arg_size): count\n" << c_scalar_mul_counts_;
  os << "total: " << c_scalar_mul_total_ << '\n';
  print_line_sep(os);
  os << "|mul_non_scalar| (level, ctxt_arg_size): count\n" << c_non_scalar_mul_counts_;
  os << "total: " << c_non_scalar_mul_total_ << '\n';
  print_line_sep(os);
  os << "|mod_switch| (level, arg_size): count\n" << mod_switch_counts_;
  os << "total: " << mod_switch_total_ << '\n';
  print_line_sep(os);
  os << "|he_add| (level, max_args_size): count\n" << he_add_counts_;
  os << "total: " << he_add_total_ << '\n';
  print_line_sep(os);
  os << "|rotation_keys|: " << rotation_keys_count_ << '\n';
  os << "|relin_keys|: " << relin_keys_count_ << '\n';

  if (outputs_details)
  {
    print_line_sep(os);
    os << "ctxt_outputs_info_ info, $id: (level, size)\n";
    os << ctxt_outputs_info_;
  }
}

void Quantifier::print_global_metrics(ostream &os) const
{
  if (!global_metrics_)
  {
    os << "global_metrics not computed\n";
    return;
  }

  os << "circuit_cost: " << circuit_cost_ << '\n';
  os << "ctxt_inputs_size: " << ctxt_inputs_size_ / 1024.0 / 1024.0 << " MB (" << ctxt_inputs_count_ << " input)\n";
  os << "ctxt_outputs_size: " << ctxt_outputs_size_ / 1024.0 / 1024.0 << " MB (" << ctxt_outputs_info_.size()
     << " output)\n";
  os << "rotation_keys_size: " << rotation_keys_size_ / 1024.0 / 1024.0 << " MB (" << rotation_keys_count_ << " key)\n";
  os << "relin_keys_size: " << relin_keys_size_ / 1024.0 / 1024.0 << " MB (" << relin_keys_count_ << " key)\n";
}

size_t Quantifier::HashDepthInfo::operator()(const DepthInfo &depth_info) const
{
  auto h = hash<double>{}(depth_info.depth_);
  util::hash_combine(h, depth_info.xdepth_);
  return h;
}

bool Quantifier::EqualDepthInfo::operator()(const DepthInfo &lhs, const DepthInfo &rhs) const
{
  return pair<double, double>{lhs.depth_, lhs.xdepth_} == pair<double, double>{rhs.depth_, rhs.xdepth_};
}

size_t Quantifier::HashCCOpInfo::operator()(const CCOpInfo &cc_op_info) const
{
  auto h = hash<double>{}(cc_op_info.opposite_level_);
  util::hash_combine(h, cc_op_info.arg1_size_);
  util::hash_combine(h, cc_op_info.arg2_size_);
  return h;
}

bool Quantifier::EqualCCOpInfo::operator()(const CCOpInfo &lhs, const CCOpInfo &rhs) const
{
  return tuple<double, double, double>{lhs.opposite_level_, lhs.arg1_size_, lhs.arg2_size_} ==
         tuple<double, double, double>{rhs.opposite_level_, rhs.arg1_size_, rhs.arg2_size_};
}

size_t Quantifier::HashCAOpInfo::operator()(const CAOpInfo &ca_op_info) const
{
  auto h = hash<double>{}(ca_op_info.opposite_level_);
  util::hash_combine(h, ca_op_info.arg_size_);
  return h;
}

bool Quantifier::EqualCAOpInfo::operator()(const CAOpInfo &lhs, const CAOpInfo &rhs) const
{
  return pair<double, double>{lhs.opposite_level_, lhs.arg_size_} ==
         pair<double, double>{rhs.opposite_level_, rhs.arg_size_};
}

Quantifier operator/(const Quantifier &lhs, const Quantifier &rhs)
{
  Quantifier result = lhs;
  if (result.depth_metrics() && rhs.depth_metrics())
  {
    result.depth_summary_ /= rhs.depth_summary();
    result.ctxt_leaves_depth_info_ /= rhs.ctxt_leaves_depth_info();
  }
  if (result.terms_classes_metrics() && rhs.terms_classes_metrics())
  {
    result.terms_count_ /= rhs.terms_count();
    result.captured_terms_count_ /= rhs.captured_terms_count();
    result.ctxt_inputs_count_ /= rhs.ctxt_inputs_count();
    result.ptxt_leaves_count_ /= rhs.ptxt_leaves_count();
    result.constants_count_ /= rhs.constants_count();
    result.pp_ops_count_ /= rhs.pp_ops_count();
    result.constants_ops_count_ /= rhs.constants_ops_count();
    result.circuit_static_cost_ /= rhs.circuit_static_cost();
    result.cc_mul_counts_ /= rhs.cc_mul_counts();
    result.cc_mul_total_ /= rhs.cc_mul_total();
    result.square_counts_ /= rhs.square_counts();
    result.square_total_ /= rhs.square_total();
    result.encrypt_count_ /= rhs.encrypt_count();
    result.relin_counts_ /= rhs.relin_counts();
    result.relin_total_ /= rhs.relin_total();
    result.rotate_counts_ /= rhs.rotate_counts();
    result.rotate_total_ /= rhs.rotate_total();
    result.cp_mul_counts_ /= rhs.cp_mul_counts();
    result.cp_mul_total_ /= rhs.cp_mul_total();
    result.c_scalar_mul_counts_ /= rhs.c_scalar_mul_counts();
    result.c_scalar_mul_total_ /= rhs.c_scalar_mul_total();
    result.c_non_scalar_mul_counts_ /= rhs.c_non_scalar_mul_counts();
    result.c_non_scalar_mul_total_ /= rhs.c_non_scalar_mul_total();
    result.mod_switch_counts_ /= rhs.mod_switch_counts();
    result.mod_switch_total_ /= rhs.mod_switch_total();
    result.he_add_counts_ /= rhs.he_add_counts();
    result.he_add_total_ /= rhs.he_add_total();
    result.ctxt_outputs_info_ /= rhs.ctxt_outputs_info();
    result.relin_keys_count_ /= rhs.relin_keys_count();
    result.rotation_keys_count_ /= rhs.rotation_keys_count();
  }
  if (result.global_metrics() && rhs.global_metrics())
  {
    result.circuit_cost_ /= rhs.circuit_cost();
    result.rotation_keys_size_ /= rhs.rotation_keys_size();
    result.relin_keys_size_ /= rhs.relin_keys_size();
    result.ctxt_inputs_size_ /= rhs.ctxt_inputs_size();
    result.ctxt_outputs_size_ /= rhs.ctxt_outputs_size();
  }
  return result;
}

Quantifier &operator/=(Quantifier &lhs, const Quantifier &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier operator-(const Quantifier &lhs, const Quantifier &rhs)
{
  Quantifier result = lhs;
  if (result.depth_metrics() && rhs.depth_metrics())
  {
    result.depth_summary_ -= rhs.depth_summary();
    result.ctxt_leaves_depth_info_ -= rhs.ctxt_leaves_depth_info();
  }
  if (result.terms_classes_metrics() && rhs.terms_classes_metrics())
  {
    result.terms_count_ -= rhs.terms_count();
    result.captured_terms_count_ -= rhs.captured_terms_count();
    result.ctxt_inputs_count_ -= rhs.ctxt_inputs_count();
    result.ptxt_leaves_count_ -= rhs.ptxt_leaves_count();
    result.constants_count_ -= rhs.constants_count();
    result.pp_ops_count_ -= rhs.pp_ops_count();
    result.constants_ops_count_ -= rhs.constants_ops_count();
    result.circuit_static_cost_ -= rhs.circuit_static_cost();
    result.cc_mul_counts_ -= rhs.cc_mul_counts();
    result.cc_mul_total_ -= rhs.cc_mul_total();
    result.square_counts_ -= rhs.square_counts();
    result.square_total_ -= rhs.square_total();
    result.encrypt_count_ -= rhs.encrypt_count();
    result.relin_counts_ -= rhs.relin_counts();
    result.relin_total_ -= rhs.relin_total();
    result.rotate_counts_ -= rhs.rotate_counts();
    result.rotate_total_ -= rhs.rotate_total();
    result.cp_mul_counts_ -= rhs.cp_mul_counts();
    result.cp_mul_total_ -= rhs.cp_mul_total();
    result.c_scalar_mul_counts_ -= rhs.c_scalar_mul_counts();
    result.c_scalar_mul_total_ -= rhs.c_scalar_mul_total();
    result.c_non_scalar_mul_counts_ = rhs.c_non_scalar_mul_counts();
    result.c_non_scalar_mul_total_ -= rhs.c_non_scalar_mul_total();
    result.mod_switch_counts_ -= rhs.mod_switch_counts();
    result.mod_switch_total_ -= rhs.mod_switch_total();
    result.he_add_counts_ -= rhs.he_add_counts();
    result.he_add_total_ -= rhs.he_add_total();
    result.ctxt_outputs_info_ -= rhs.ctxt_outputs_info();
    result.relin_keys_count_ -= rhs.relin_keys_count();
    result.rotation_keys_count_ -= rhs.rotation_keys_count();
  }
  if (result.global_metrics() && rhs.global_metrics())
  {
    result.circuit_cost_ -= rhs.circuit_cost();
    result.rotation_keys_size_ -= rhs.rotation_keys_size();
    result.relin_keys_size_ -= rhs.relin_keys_size();
    result.ctxt_inputs_size_ -= rhs.ctxt_inputs_size();
    result.ctxt_outputs_size_ -= rhs.ctxt_outputs_size();
  }
  return result;
}

Quantifier &operator-=(Quantifier &lhs, const Quantifier &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier operator*(const Quantifier &lhs, int coeff)
{
  Quantifier result = lhs;
  if (result.depth_metrics())
  {
    result.depth_summary_ *= coeff;
    result.ctxt_leaves_depth_info_ *= coeff;
  }
  if (result.terms_classes_metrics())
  {

    result.terms_count_ *= coeff;
    result.captured_terms_count_ *= coeff;
    result.ctxt_inputs_count_ *= coeff;
    result.ptxt_leaves_count_ *= coeff;
    result.constants_count_ *= coeff;
    result.pp_ops_count_ *= coeff;
    result.constants_ops_count_ *= coeff;
    result.circuit_static_cost_ *= coeff;
    result.cc_mul_counts_ *= coeff;
    result.cc_mul_total_ *= coeff;
    result.square_counts_ *= coeff;
    result.square_total_ *= coeff;
    result.encrypt_count_ *= coeff;
    result.relin_counts_ *= coeff;
    result.relin_total_ *= coeff;
    result.rotate_counts_ *= coeff;
    result.rotate_total_ *= coeff;
    result.cp_mul_counts_ *= coeff;
    result.cp_mul_total_ *= coeff;
    result.c_scalar_mul_counts_ *= coeff;
    result.c_scalar_mul_total_ *= coeff;
    result.c_non_scalar_mul_counts_ *= coeff;
    result.c_non_scalar_mul_total_ *= coeff;
    result.mod_switch_counts_ *= coeff;
    result.mod_switch_total_ *= coeff;
    result.he_add_counts_ *= coeff;
    result.he_add_total_ *= coeff;
    result.ctxt_outputs_info_ *= coeff;
    result.relin_keys_count_ *= coeff;
    result.rotation_keys_count_ *= coeff;
  }
  if (result.global_metrics())
  {
    result.circuit_cost_ *= coeff;
    result.rotation_keys_size_ *= coeff;
    result.relin_keys_size_ *= coeff;
    result.ctxt_inputs_size_ *= coeff;
    result.ctxt_outputs_size_ *= coeff;
  }
  return result;
}

Quantifier operator*(int coeff, const Quantifier &rhs)
{
  return rhs * coeff;
}

Quantifier operator*=(Quantifier &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::DepthSummary operator/(const Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs)
{
  auto result = lhs;
  result.min_depth_ /= rhs.min_depth_;
  result.min_xdepth_ /= rhs.min_xdepth_;
  result.avg_depth_ /= rhs.avg_depth_;
  result.avg_xdepth_ /= rhs.avg_xdepth_;
  result.max_depth_ /= rhs.max_depth_;
  result.max_xdepth_ /= rhs.max_xdepth_;
  return result;
}

Quantifier::DepthSummary &operator/=(Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::DepthSummary operator-(const Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs)
{
  auto result = lhs;
  result.min_depth_ -= rhs.min_depth_;
  result.min_xdepth_ -= rhs.min_xdepth_;
  result.avg_depth_ -= rhs.avg_depth_;
  result.avg_xdepth_ -= rhs.avg_xdepth_;
  result.max_depth_ -= rhs.max_depth_;
  result.max_xdepth_ -= rhs.max_xdepth_;
  return result;
}

Quantifier::DepthSummary &operator-=(Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::DepthSummary operator*(const Quantifier::DepthSummary &lhs, int coeff)
{
  auto result = lhs;
  result.min_depth_ *= coeff;
  result.min_xdepth_ *= coeff;
  result.avg_depth_ *= coeff;
  result.avg_xdepth_ *= coeff;
  result.max_depth_ *= coeff;
  result.max_xdepth_ *= coeff;
  return result;
}

Quantifier::DepthSummary operator*(int coeff, const Quantifier::DepthSummary &rhs)
{
  return rhs * coeff;
}

Quantifier::DepthSummary operator*=(Quantifier::DepthSummary &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::CtxtTermsDepthInfo operator/(
  const Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    auto term = e.first;
    if (auto rhs_e_it = rhs.find(term); rhs_e_it != rhs.end())
      e.second /= rhs_e_it->second;
  }
  return result;
}

Quantifier::CtxtTermsDepthInfo &operator/=(
  Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::CtxtTermsDepthInfo operator-(
  const Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs)
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

Quantifier::CtxtTermsDepthInfo &operator-=(
  Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::CtxtTermsDepthInfo operator*(const Quantifier::CtxtTermsDepthInfo &lhs, int coeff)
{
  auto result = lhs;
  for (auto &e : result)
    e.second *= coeff;
  return result;
}

Quantifier::CtxtTermsDepthInfo operator*(int coeff, const Quantifier::CtxtTermsDepthInfo &rhs)
{
  return rhs * coeff;
}

Quantifier::CtxtTermsDepthInfo operator*=(Quantifier::CtxtTermsDepthInfo &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::DepthInfo operator/(const Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs)
{
  auto result = lhs;
  result.depth_ /= rhs.depth_;
  result.xdepth_ /= rhs.xdepth_;
  return result;
}

Quantifier::DepthInfo &operator/=(Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::DepthInfo operator-(const Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs)
{
  auto result = lhs;
  result.depth_ -= rhs.depth_;
  result.xdepth_ -= rhs.xdepth_;
  return result;
}

Quantifier::DepthInfo &operator-=(Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::DepthInfo operator*(const Quantifier::DepthInfo &lhs, int coeff)
{
  auto result = lhs;
  result.depth_ *= coeff;
  result.xdepth_ *= coeff;
  return result;
}

Quantifier::DepthInfo operator*(int coeff, const Quantifier::DepthInfo &rhs)
{
  return rhs * coeff;
}

Quantifier::DepthInfo operator*=(Quantifier::DepthInfo &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::CCOpsCounts operator/(const Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    if (auto it = rhs.find(e.first); it != rhs.end())
      e.second /= it->second;
    else
      e.second = numeric_limits<double>::infinity();
  }
  for (const auto &e : rhs)
  {
    if (result.find(e.first) == result.end())
      result.emplace(e.first, 0 / e.second);
  }
  return result;
}

Quantifier::CCOpsCounts &operator/=(Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::CCOpsCounts operator-(const Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs)
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

Quantifier::CCOpsCounts &operator-=(Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::CCOpsCounts operator*(const Quantifier::CCOpsCounts &lhs, int coeff)
{
  auto result = lhs;
  for (auto &e : result)
    e.second *= coeff;

  return result;
}

Quantifier::CCOpsCounts operator*(int coeff, const Quantifier::CCOpsCounts &rhs)
{
  return rhs * coeff;
}

Quantifier::CCOpsCounts operator*=(Quantifier::CCOpsCounts &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::CAOpsCounts operator/(const Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    if (auto it = rhs.find(e.first); it != rhs.end())
      e.second /= it->second;
    else
      e.second = numeric_limits<double>::infinity();
  }
  for (const auto &e : rhs)
  {
    if (result.find(e.first) == result.end())
      result.emplace(e.first, 0 / e.second);
  }
  return result;
}

Quantifier::CAOpsCounts &operator/=(Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::CAOpsCounts operator-(const Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs)
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

Quantifier::CAOpsCounts &operator-=(Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::CAOpsCounts operator*(const Quantifier::CAOpsCounts &lhs, int coeff)
{
  auto result = lhs;
  for (auto &e : result)
    e.second *= coeff;

  return result;
}

Quantifier::CAOpsCounts operator*(int coeff, const Quantifier::CAOpsCounts &rhs)
{
  return rhs * coeff;
}

Quantifier::CAOpsCounts operator*=(Quantifier::CAOpsCounts &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::CtxtTermsInfo operator/(const Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs)
{
  auto result = lhs;
  for (auto &e : result)
  {
    auto term = e.first;
    if (auto rhs_e_it = rhs.find(term); rhs_e_it != rhs.end())
      e.second /= rhs_e_it->second;
  }
  return result;
}

Quantifier::CtxtTermsInfo &operator/=(Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::CtxtTermsInfo operator-(const Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs)
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

Quantifier::CtxtTermsInfo &operator-=(Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::CtxtTermsInfo operator*(const Quantifier::CtxtTermsInfo &lhs, int coeff)
{
  auto result = lhs;
  for (auto &e : result)
    e.second *= coeff;

  return result;
}

Quantifier::CtxtTermsInfo operator*(int coeff, const Quantifier::CtxtTermsInfo &rhs)
{
  return rhs * coeff;
}

Quantifier::CtxtTermsInfo operator*=(Quantifier::CtxtTermsInfo &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

Quantifier::CtxtTermInfo operator/(const Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs)
{
  auto result = lhs;
  result.opposite_level_ /= rhs.opposite_level_;
  result.size_ /= rhs.size_;
  return result;
}

Quantifier::CtxtTermInfo &operator/=(Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs)
{
  lhs = lhs / rhs;
  return lhs;
}

Quantifier::CtxtTermInfo operator-(const Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs)
{
  auto result = lhs;
  result.opposite_level_ -= rhs.opposite_level_;
  result.size_ -= rhs.size_;
  return result;
}

Quantifier::CtxtTermInfo &operator-=(Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

Quantifier::CtxtTermInfo operator*(const Quantifier::CtxtTermInfo &lhs, int coeff)
{
  auto result = lhs;
  result.opposite_level_ *= coeff;
  result.size_ *= coeff;
  return result;
}

Quantifier::CtxtTermInfo operator*(int coeff, const Quantifier::CtxtTermInfo &rhs)
{
  return rhs * coeff;
}

Quantifier::CtxtTermInfo operator*=(Quantifier::CtxtTermInfo &lhs, int coeff)
{
  lhs = lhs * coeff;
  return lhs;
}

ostream &operator<<(ostream &os, const Quantifier &quantifier)
{
  print_line_sep(os);
  os << "he_depth_info\n";
  quantifier.print_depth_info(os, true);
  print_line_sep(os);
  os << "terms_classes_info\n";
  quantifier.print_terms_classes_info(os, true);
  return os;
}

ostream &operator<<(ostream &os, const Quantifier::CtxtTermsDepthInfo &ctxt_terms_depth_info)
{
  for (const auto &e : ctxt_terms_depth_info)
    os << '$' << e.first->id() << ": (" << e.second << ")\n";
  return os;
}

ostream &operator<<(ostream &os, const Quantifier::DepthInfo &ctxt_term_depth_info)
{
  return os << ctxt_term_depth_info.depth_ << ", " << ctxt_term_depth_info.xdepth_;
}

ostream &operator<<(ostream &os, const Quantifier::CCOpsCounts &cc_ops_counts)
{
  for (const auto &e : cc_ops_counts)
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.arg1_size_ << ", " << e.first.arg2_size_
       << "): " << e.second << '\n';
  return os;
}

ostream &operator<<(ostream &os, const Quantifier::CAOpsCounts &ca_op_counts)
{
  for (const auto &e : ca_op_counts)
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.arg_size_ << "): " << e.second << '\n';
  return os;
}

ostream &operator<<(ostream &os, const Quantifier::CtxtTermsInfo &ctxt_terms_info)
{
  for (const auto &e : ctxt_terms_info)
    os << '$' << e.first->id() << ": (" << e.second << ")\n";
  return os;
}

ostream &operator<<(ostream &os, const Quantifier::CtxtTermInfo &ctxt_term_info)
{
  return os << ctxt_term_info.opposite_level_ << ", " << ctxt_term_info.size_;
}

void print_line_sep(ostream &os)
{
  os << string(100, '-') << '\n';
}
} // namespace fheco::util
