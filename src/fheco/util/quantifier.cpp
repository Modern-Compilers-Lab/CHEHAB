#include "fheco/util/quantifier.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>

using namespace std;

void print_line_sep(ostream &os)
{
  os << string(100, '-') << '\n';
}

namespace fheco::util
{
void Quantifier::run_analysis()
{
  compute_he_depth_info();
  count_terms_classes();
}

void Quantifier::compute_he_depth_info()
{
  ctxt_leaves_depth_info_.clear();

  stack<pair<const ir::Term *, DepthInfo>> dfs;
  for (auto output_term : func_->data_flow().outputs())
  {
    if (output_term->type() == ir::TermType::cipher && output_term->is_source())
      dfs.push({output_term, DepthInfo{0, 0}});

    while (!dfs.empty())
    {
      auto [top_term, top_ctxt_info] = dfs.top();
      dfs.pop();
      if (top_term->is_leaf() || top_term->op_code().type() == ir::OpCode::Type::encrypt)
      {
        auto [it, inserted] = ctxt_leaves_depth_info_.emplace(top_term->id(), DepthInfo{0, 0});
        int new_xdepth = max(it->second.xdepth_, top_ctxt_info.xdepth_);
        int new_depth = max(it->second.depth_, top_ctxt_info.depth_);
        it->second = DepthInfo{new_xdepth, new_depth};
        continue;
      }
      int operands_xdepth = top_ctxt_info.xdepth_;
      int operands_depth = top_ctxt_info.depth_ + 1;
      if (top_term->op_code().type() == ir::OpCode::Type::mul || top_term->op_code().type() == ir::OpCode::Type::square)
        ++operands_xdepth;
      for (const auto operand : top_term->operands())
      {
        if (operand->type() == ir::TermType::cipher)
          dfs.push({operand, DepthInfo{operands_xdepth, operands_depth}});
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
}

void Quantifier::count_terms_classes()
{
  all_terms_count_ = 0;
  captured_terms_count_ = 0;
  ptxt_leaves_count_ = 0;
  pp_ops_count_ = 0;
  relin_keys_count_ = 0;
  rotation_keys_count_ = 0;
  ctxt_leaves_count_ = 0;
  cc_mul_count_.clear();
  he_square_count_.clear();
  encrypt_count_ = 0;
  relin_count_.clear();
  he_rotate_count_.clear();
  cp_mul_count_.clear();
  mod_switch_count_.clear();
  he_add_sub_negate_count_.clear();

  struct CtxtInfo
  {
    size_t opposed_level_;
    size_t size_;
  };
  unordered_map<size_t, CtxtInfo> ctxts_info;

  unordered_set<size_t> rotation_keys_steps{};

  for (const auto &term : func_->get_top_sorted_terms())
  {
    ++all_terms_count_;
    if (term->is_operation())
    {
      if (term->type() == ir::TermType::cipher)
      {
        if (term->op_code().type() == ir::OpCode::Type::mul)
        {
          if (term->operands()[0] == term->operands()[1])
          {
            const auto &arg1_info = ctxts_info.find(term->operands()[0]->id())->second;
            const auto &arg2_info = ctxts_info.find(term->operands()[1]->id())->second;
            if (arg1_info.opposed_level_ != arg2_info.opposed_level_)
              throw logic_error("he operation with operands having different levels");

            ctxts_info.emplace(term->id(), CtxtInfo{arg1_info.opposed_level_, arg1_info.size_ + arg2_info.size_ - 1});
            ++captured_terms_count_;
            auto [it, inserted] = cc_mul_count_.emplace(
              CCOpInfo{
                arg1_info.opposed_level_, min(arg1_info.size_, arg2_info.size_), max(arg1_info.size_, arg2_info.size_)},
              1);
            if (!inserted)
              ++it->second;
          }
          else
          {
            const auto &ctxt_arg_info = (term->operands()[0]->type() == ir::TermType::cipher)
                                          ? ctxts_info.find(term->operands()[0]->id())->second
                                          : ctxts_info.find(term->operands()[1]->id())->second;

            ctxts_info.emplace(term->id(), CtxtInfo{ctxt_arg_info.opposed_level_, ctxt_arg_info.size_});
            ++captured_terms_count_;
            auto [it, inserted] = cp_mul_count_.emplace(CAOpInfo{ctxt_arg_info.opposed_level_, ctxt_arg_info.size_}, 1);
            if (!inserted)
              ++it->second;
          }
        }
        else if (term->op_code().type() == ir::OpCode::Type::square)
        {
          const auto &arg_info = ctxts_info.find(term->operands()[0]->id())->second;
          ctxts_info.emplace(term->id(), CtxtInfo{arg_info.opposed_level_, 2 * arg_info.size_ - 1});
          ++captured_terms_count_;
          auto [it, inserted] = he_square_count_.emplace(CAOpInfo{arg_info.opposed_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::encrypt)
        {
          ctxts_info.emplace(term->id(), CtxtInfo{0, 2});
          ++captured_terms_count_;
          ++encrypt_count_;
        }
        else if (term->op_code().type() == ir::OpCode::Type::relin)
        {
          const auto &arg_info = ctxts_info.find(term->operands()[0]->id())->second;
          ctxts_info.emplace(term->id(), CtxtInfo{arg_info.opposed_level_, 2});
          relin_keys_count_ = max(relin_keys_count_, static_cast<int64_t>(arg_info.size_ - 2));
          ++captured_terms_count_;
          auto [it, inserted] = relin_count_.emplace(CAOpInfo{arg_info.opposed_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::rotate)
        {
          const auto &arg_info = ctxts_info.find(term->operands()[0]->id())->second;
          ctxts_info.emplace(term->id(), CtxtInfo{arg_info.opposed_level_, arg_info.size_});
          rotation_keys_steps.insert(term->op_code().steps());
          ++captured_terms_count_;
          auto [it, inserted] = he_rotate_count_.emplace(CAOpInfo{arg_info.opposed_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::mod_switch)
        {
          const auto &arg_info = ctxts_info.find(term->operands()[0]->id())->second;
          ctxts_info.emplace(term->id(), CtxtInfo{arg_info.opposed_level_ + 1, arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] = mod_switch_count_.emplace(CAOpInfo{arg_info.opposed_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::add || term->op_code().type() == ir::OpCode::Type::sub)
        {
          CtxtInfo max_arg_info;
          if (term->operands()[0]->type() == term->operands()[1]->type())
          {
            const auto &arg1_info = ctxts_info.find(term->operands()[0]->id())->second;
            const auto &arg2_info = ctxts_info.find(term->operands()[1]->id())->second;
            if (arg1_info.opposed_level_ != arg2_info.opposed_level_)
              throw logic_error("he operation with operands having different levels");

            max_arg_info.opposed_level_ = arg1_info.opposed_level_;
            max_arg_info.size_ = max(arg1_info.size_, arg2_info.size_);
          }
          else
          {
            const auto &ctxt_arg_info = (term->operands()[0]->type() == ir::TermType::cipher)
                                          ? ctxts_info.find(term->operands()[0]->id())->second
                                          : ctxts_info.find(term->operands()[1]->id())->second;
            max_arg_info = ctxt_arg_info;
          }
          ctxts_info.emplace(term->id(), CtxtInfo{max_arg_info.opposed_level_, max_arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] =
            he_add_sub_negate_count_.emplace(CAOpInfo{max_arg_info.opposed_level_, max_arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else if (term->op_code().type() == ir::OpCode::Type::negate)
        {
          const auto &arg_info = ctxts_info.find(term->operands()[0]->id())->second;
          ctxts_info.emplace(term->id(), CtxtInfo{arg_info.opposed_level_, arg_info.size_});
          ++captured_terms_count_;
          auto [it, inserted] = he_add_sub_negate_count_.emplace(CAOpInfo{arg_info.opposed_level_, arg_info.size_}, 1);
          if (!inserted)
            ++it->second;
        }
        else
          throw logic_error("unhandled he operation");
      }
      else
        ++pp_ops_count_;
    }
    else
    {
      if (term->type() == ir::TermType::cipher)
      {
        ctxts_info.emplace(term->id(), CtxtInfo{0, 2});
        ++captured_terms_count_;
        ++ctxt_leaves_count_;
      }
      else
      {
        ++captured_terms_count_;
        ++ptxt_leaves_count_;
      }
    }
  }
  rotation_keys_count_ = rotation_keys_steps.size();
}

Quantifier Quantifier::operator-(const Quantifier &other) const
{
  Quantifier result = *this;
  result.he_depth_summary_ -= other.he_depth_summary_;
  result.all_terms_count_ -= other.all_terms_count_;
  result.captured_terms_count_ -= other.captured_terms_count_;
  result.ptxt_leaves_count_ -= other.ptxt_leaves_count_;
  result.pp_ops_count_ -= other.pp_ops_count_;
  result.relin_keys_count_ -= other.relin_keys_count_;
  result.rotation_keys_count_ -= other.rotation_keys_count_;
  result.ctxt_leaves_count_ -= other.ctxt_leaves_count_;
  result.cc_mul_count_ -= other.cc_mul_count_;
  result.he_square_count_ -= other.he_square_count_;
  result.encrypt_count_ -= other.encrypt_count_;
  result.relin_count_ -= other.relin_count_;
  result.he_rotate_count_ -= other.he_rotate_count_;
  result.cp_mul_count_ -= other.cp_mul_count_;
  result.mod_switch_count_ -= other.mod_switch_count_;
  result.he_add_sub_negate_count_ -= other.he_add_sub_negate_count_;
  return result;
}

Quantifier &Quantifier::operator-=(const Quantifier &other)
{
  *this = *this - other;
  return *this;
}

void Quantifier::print_info(std::ostream &os)
{
  print_line_sep(os);
  os << "he_depth_info\n";
  print_he_depth_info(os);
  print_line_sep(os);
  os << "terms_classes_info\n";
  print_terms_classes_info(os);
}

void Quantifier::print_he_depth_info(std::ostream &os) const
{
  os << "ctxt_leaves_*: (xdepth, depth)\n";
  os << "min: (" << he_depth_summary_.min_xdepth_ << ", " << he_depth_summary_.min_depth_ << ")\n";
  os << "avg: (" << he_depth_summary_.avg_xdepth_ << ", " << he_depth_summary_.avg_depth_ << ")\n";
  os << "max: (" << he_depth_summary_.max_xdepth_ << ", " << he_depth_summary_.max_depth_ << ")\n";
  os << ctxt_leaves_depth_info_;
}

void Quantifier::print_terms_classes_info(ostream &os) const
{
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
}

size_t Quantifier::HashCCOpInfo::operator()(const CCOpInfo &cc_op_info) const
{
  size_t h = hash<size_t>{}(cc_op_info.opposite_level_);
  ir::hash_combine(h, cc_op_info.size1_);
  ir::hash_combine(h, cc_op_info.size2_);
  return h;
}
bool Quantifier::EqualCCOpInfo::operator()(const CCOpInfo &lhs, const CCOpInfo &rhs) const
{
  return tuple<size_t, size_t, size_t>{lhs.opposite_level_, lhs.size1_, lhs.size2_} ==
         tuple<size_t, size_t, size_t>{rhs.opposite_level_, rhs.size1_, rhs.size2_};
}

size_t Quantifier::HashCAOpInfo::operator()(const CAOpInfo &ca_op_info) const
{
  size_t h = hash<size_t>{}(ca_op_info.opposite_level_);
  ir::hash_combine(h, ca_op_info.size_);
  return h;
}
bool Quantifier::EqualCAOpInfo::operator()(const CAOpInfo &lhs, const CAOpInfo &rhs) const
{
  return pair<size_t, size_t>{lhs.opposite_level_, lhs.size_} == pair<size_t, size_t>{rhs.opposite_level_, rhs.size_};
}
} // namespace fheco::util

namespace std
{
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

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CCOpCount &cc_op_count)
{
  int64_t total = 0;
  for (const auto &e : cc_op_count)
  {
    total += e.second;
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.size1_ << ", " << e.first.size2_ << "): " << e.second
       << '\n';
  }
  os << "total: " << total << '\n';
  return os;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CAOpCount &ca_op_count)
{
  int64_t total = 0;
  for (const auto &e : ca_op_count)
  {
    total += e.second;
    os << "(L-" << e.first.opposite_level_ << ", " << e.first.size_ << "): " << e.second << '\n';
  }
  os << "total: " << total << '\n';
  return os;
}

ostream &operator<<(ostream &os, const fheco::util::Quantifier::TermDepthInfo &terms_depth_info)
{
  for (const auto &e : terms_depth_info)
    os << '$' << e.first << ": (" << e.second.xdepth_ << ", " << e.second.depth_ << ")\n";
  return os;
}
} // namespace std
