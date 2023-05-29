#pragma once

#include "fheco/param_select/encryption_params.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
class Quantifier
{
public:
  struct DepthSummary
  {
    int min_xdepth_;
    int min_depth_;
    double avg_xdepth_;
    double avg_depth_;
    int max_xdepth_;
    int max_depth_;
  };

  struct CtxtTermDepthInfo
  {
    int xdepth_;
    int depth_;
  };

  using CtxtTermsDepthInfo =
    std::unordered_map<const ir::Term *, CtxtTermDepthInfo, ir::Term::HashPtr, ir::Term::EqualPtr>;

  // ctxt ctxt operation info
  struct CCOpInfo
  {
    std::int64_t opposite_level_;
    std::int64_t arg1_size_;
    std::int64_t arg2_size_;
  };
  struct HashCCOpInfo
  {
    std::size_t operator()(const CCOpInfo &cc_op) const;
  };
  struct EqualCCOpInfo
  {
    bool operator()(const CCOpInfo &lhs, const CCOpInfo &rhs) const;
  };
  using CCOpCount = std::unordered_map<CCOpInfo, std::int64_t, HashCCOpInfo, EqualCCOpInfo>;

  // ctxt any operation info
  struct CAOpInfo
  {
    std::int64_t opposite_level_;
    std::int64_t arg_size_;
  };
  struct HashCAOpInfo
  {
    std::size_t operator()(const CAOpInfo &ca_op_info) const;
  };
  struct EqualCAOpInfo
  {
    bool operator()(const CAOpInfo &lhs, const CAOpInfo &rhs) const;
  };
  using CAOpCount = std::unordered_map<CAOpInfo, std::int64_t, HashCAOpInfo, EqualCAOpInfo>;

  struct CtxtTermInfo
  {
    std::int64_t opposite_level_;
    std::int64_t size_;
  };

  using CtxtTermsInfo = std::unordered_map<const ir::Term *, CtxtTermInfo, ir::Term::HashPtr, ir::Term::EqualPtr>;

  Quantifier(std::shared_ptr<ir::Func> func) : func_{std::move(func)} {}

  void run_all_analysis(const param_select::EncryptionParams &params);

  void run_all_analysis();

  void compute_he_depth_info();

  void count_terms_classes();

  void compute_global_metrics(const param_select::EncryptionParams &params);

  Quantifier operator-(const Quantifier &other) const;

  Quantifier &operator-=(const Quantifier &other);

  void print_info(std::ostream &os, bool depth_details = false);

  void print_he_depth_info(std::ostream &os, bool details) const;

  void print_terms_classes_info(std::ostream &os) const;

  void print_global_metrics(std::ostream &os) const;

  inline const std::shared_ptr<ir::Func> func() const { return func_; }

  inline bool depth_metrics() const { return depth_metrics_; }

  inline const DepthSummary &he_depth_summary() const { return he_depth_summary_; }

  inline const CtxtTermsDepthInfo &ctxt_leaves_depth_info() const { return ctxt_leaves_depth_info_; }

  inline bool terms_classes_metrics() const { return terms_classes_metrics_; }

  inline std::int64_t relin_keys_count() const { return relin_keys_count_; }

  inline std::int64_t rotation_keys_count() const { return rotation_keys_count_; }

  inline std::int64_t all_terms_count() const { return all_terms_count_; }

  inline std::int64_t captured_terms_count() const { return captured_terms_count_; }

  inline std::int64_t ptxt_leaves_count() const { return ptxt_leaves_count_; }

  inline std::int64_t pp_ops_count() const { return pp_ops_count_; }

  inline std::int64_t ctxt_leaves_count() const { return ctxt_leaves_count_; }

  inline const CCOpCount &cc_mul_count() const { return cc_mul_count_; }

  inline const CAOpCount &he_square_count() const { return he_square_count_; }

  inline std::int64_t encrypt_count() const { return encrypt_count_; }

  inline const CAOpCount &relin_count() const { return relin_count_; }

  inline const CAOpCount &he_rotate_count() const { return he_rotate_count_; }

  inline const CAOpCount &cp_mul_count() const { return cp_mul_count_; }

  inline const CAOpCount &mod_switch_count() const { return mod_switch_count_; }

  inline const CAOpCount &he_add_sub_negate_count() const { return he_add_sub_negate_count_; }

  inline const CtxtTermsInfo &ctxt_output_terms_info() const { return ctxt_output_terms_info_; }

  inline std::int64_t circuit_static_cost() const { return circuit_static_cost_; }

  inline bool global_metrics() const { return global_metrics_; }

  inline std::int64_t circuit_cost() const { return circuit_cost_; }

  inline std::int64_t rotation_keys_total_size() const { return rotation_keys_total_size_; }

  inline std::int64_t relin_keys_total_size() const { return relin_keys_total_size_; }

  inline std::int64_t ctxt_input_terms_total_size() const { return ctxt_input_terms_total_size_; }

  inline std::int64_t ctxt_input_terms_count() const { return ctxt_input_terms_count_; }

  inline std::int64_t ctxt_output_terms_total_size() const { return ctxt_output_terms_total_size_; }

private:
  std::shared_ptr<ir::Func> func_;

  bool depth_metrics_ = false;

  DepthSummary he_depth_summary_{};

  CtxtTermsDepthInfo ctxt_leaves_depth_info_{};

  bool terms_classes_metrics_ = false;

  std::int64_t relin_keys_count_ = 0;

  std::int64_t rotation_keys_count_ = 0;

  std::int64_t all_terms_count_ = 0;

  std::int64_t captured_terms_count_ = 0;

  std::int64_t ptxt_leaves_count_ = 0;

  std::int64_t pp_ops_count_ = 0;

  std::int64_t ctxt_leaves_count_ = 0;

  CCOpCount cc_mul_count_{};

  CAOpCount he_square_count_{};

  std::int64_t encrypt_count_ = 0;

  CAOpCount relin_count_{};

  CAOpCount he_rotate_count_{};

  CAOpCount cp_mul_count_{};

  CAOpCount mod_switch_count_{};

  CAOpCount he_add_sub_negate_count_{};

  CtxtTermsInfo ctxt_output_terms_info_{};

  std::int64_t circuit_static_cost_ = 0;

  bool global_metrics_ = false;

  std::int64_t circuit_cost_ = 0;

  std::int64_t rotation_keys_total_size_ = 0;

  std::int64_t relin_keys_total_size_ = 0;

  std::int64_t ctxt_input_terms_total_size_ = 0;

  std::int64_t ctxt_input_terms_count_ = 0;

  std::int64_t ctxt_output_terms_total_size_ = 0;
};
} // namespace fheco::util

inline void print_line_sep(std::ostream &os)
{
  os << std::string(100, '-') << '\n';
}

namespace std
{
fheco::util::Quantifier::DepthSummary operator-(
  const fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs);

fheco::util::Quantifier::DepthSummary &operator-=(
  fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs);

fheco::util::Quantifier::CtxtTermsDepthInfo operator-(
  const fheco::util::Quantifier::CtxtTermsDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermsDepthInfo &rhs);

fheco::util::Quantifier::CtxtTermsDepthInfo &operator-=(
  fheco::util::Quantifier::CtxtTermsDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermsDepthInfo &rhs);

fheco::util::Quantifier::CtxtTermDepthInfo operator-(
  const fheco::util::Quantifier::CtxtTermDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermDepthInfo &rhs);

fheco::util::Quantifier::CtxtTermDepthInfo &operator-=(
  fheco::util::Quantifier::CtxtTermDepthInfo &lhs, const fheco::util::Quantifier::CtxtTermDepthInfo &rhs);

fheco::util::Quantifier::CCOpCount operator-(
  const fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs);

fheco::util::Quantifier::CCOpCount &operator-=(
  fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs);

fheco::util::Quantifier::CAOpCount operator-(
  const fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs);

fheco::util::Quantifier::CAOpCount &operator-=(
  fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs);

fheco::util::Quantifier::CtxtTermsInfo operator-(
  const fheco::util::Quantifier::CtxtTermsInfo &lhs, const fheco::util::Quantifier::CtxtTermsInfo &rhs);

fheco::util::Quantifier::CtxtTermsInfo &operator-=(
  fheco::util::Quantifier::CtxtTermsInfo &lhs, const fheco::util::Quantifier::CtxtTermsInfo &rhs);

fheco::util::Quantifier::CtxtTermInfo operator-(
  const fheco::util::Quantifier::CtxtTermInfo &lhs, const fheco::util::Quantifier::CtxtTermInfo &rhs);

fheco::util::Quantifier::CtxtTermInfo &operator-=(
  fheco::util::Quantifier::CtxtTermInfo &lhs, const fheco::util::Quantifier::CtxtTermInfo &rhs);

ostream &operator<<(ostream &os, const fheco::util::Quantifier &quantifier);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermsDepthInfo &ctxt_terms_depth_info);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermDepthInfo &ctxt_term_depth_info);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CCOpCount &cc_op_count);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CAOpCount &ca_op_count);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermsInfo &ctxt_terms_info);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CtxtTermInfo &ctxt_term_info);
} // namespace std
