#pragma once

#include "fheco/param_select/enc_params.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
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
    double min_depth_;
    double min_xdepth_;
    double avg_depth_;
    double avg_xdepth_;
    double max_depth_;
    double max_xdepth_;
  };

  struct DepthInfo
  {
    double depth_;
    double xdepth_;
  };

  struct HashDepthInfo
  {
    std::size_t operator()(const DepthInfo &depth_info) const;
  };

  struct EqualDepthInfo
  {
    bool operator()(const DepthInfo &lhs, const DepthInfo &rhs) const;
  };

  using CtxtTermsDepthInfo = std::unordered_map<const ir::Term *, DepthInfo, ir::Term::HashPtr, ir::Term::EqualPtr>;

  // ctxt-ctxt operation info
  struct CCOpInfo
  {
    double opposite_level_;
    double arg1_size_;
    double arg2_size_;
  };

  struct HashCCOpInfo
  {
    std::size_t operator()(const CCOpInfo &cc_op) const;
  };

  struct EqualCCOpInfo
  {
    bool operator()(const CCOpInfo &lhs, const CCOpInfo &rhs) const;
  };

  using CCOpsCounts = std::unordered_map<CCOpInfo, double, HashCCOpInfo, EqualCCOpInfo>;

  // ctxt-any operation info
  struct CAOpInfo
  {
    double opposite_level_;
    double arg_size_;
  };

  struct HashCAOpInfo
  {
    std::size_t operator()(const CAOpInfo &ca_op_info) const;
  };

  struct EqualCAOpInfo
  {
    bool operator()(const CAOpInfo &lhs, const CAOpInfo &rhs) const;
  };

  using CAOpsCounts = std::unordered_map<CAOpInfo, double, HashCAOpInfo, EqualCAOpInfo>;

  struct CtxtTermInfo
  {
    double opposite_level_;
    double size_;
  };

  using CtxtTermsInfo = std::unordered_map<const ir::Term *, CtxtTermInfo, ir::Term::HashPtr, ir::Term::EqualPtr>;

  Quantifier(std::shared_ptr<ir::Func> func) : func_{std::move(func)} {}

  void run_all_analysis(const param_select::EncParams &params);

  void run_all_analysis();

  void compute_depth_info();

  void count_terms_classes();

  void compute_global_metrics(const param_select::EncParams &params);

  void print_info(std::ostream &os, bool depth_details = false, bool outputs_details = false);

  void print_depth_info(std::ostream &os, bool details) const;

  void print_terms_classes_info(std::ostream &os, bool outputs_details) const;

  void print_global_metrics(std::ostream &os) const;

  inline const std::shared_ptr<ir::Func> func() const { return func_; }

  inline bool depth_metrics() const { return depth_metrics_; }

  inline const DepthSummary &depth_summary() const { return depth_summary_; }

  inline const CtxtTermsDepthInfo &ctxt_leaves_depth_info() const { return ctxt_leaves_depth_info_; }

  inline bool terms_classes_metrics() const { return terms_classes_metrics_; }

  inline double terms_count() const { return terms_count_; }

  inline double captured_terms_count() const { return captured_terms_count_; }

  inline double ctxt_inputs_count() const { return ctxt_inputs_count_; }

  inline double ptxt_leaves_count() const { return ptxt_leaves_count_; }

  inline double constants_count() const { return constants_count_; }

  inline double pp_ops_count() const { return pp_ops_count_; }

  inline double constants_ops_count() const { return constants_ops_count_; }

  inline double circuit_static_cost() const { return circuit_static_cost_; }

  inline const CCOpsCounts &cc_mul_counts() const { return cc_mul_counts_; }

  inline double cc_mul_total() const { return cc_mul_total_; }

  inline const CAOpsCounts &square_counts() const { return square_counts_; }

  inline double square_total() const { return square_total_; }

  inline double encrypt_count() const { return encrypt_count_; }

  inline const CAOpsCounts &relin_counts() const { return relin_counts_; }

  inline double relin_total() const { return relin_total_; }

  inline const CAOpsCounts &rotate_counts() const { return rotate_counts_; }

  inline double rotate_total() const { return rotate_total_; }

  inline const CAOpsCounts &cp_mul_counts() const { return cp_mul_counts_; }

  inline double cp_mul_total() const { return cp_mul_total_; }

  inline const CAOpsCounts &c_scalar_mul_counts() const { return c_scalar_mul_counts_; }

  inline double c_scalar_mul_total() const { return c_scalar_mul_total_; }

  inline const CAOpsCounts &c_non_scalar_mul_counts() const { return c_non_scalar_mul_counts_; }

  inline double c_non_scalar_mul_total() const { return c_non_scalar_mul_total_; }

  inline const CAOpsCounts &mod_switch_counts() const { return mod_switch_counts_; }

  inline double mod_switch_total() const { return mod_switch_total_; }

  inline const CAOpsCounts &he_add_counts() const { return he_add_counts_; }

  inline double he_add_total() const { return he_add_total_; }

  inline const CtxtTermsInfo &ctxt_outputs_info() const { return ctxt_outputs_info_; }

  inline double relin_keys_count() const { return relin_keys_count_; }

  inline double rotation_keys_count() const { return rotation_keys_count_; }

  inline bool global_metrics() const { return global_metrics_; }

  inline double circuit_cost() const { return circuit_cost_; }

  inline double rotation_keys_size() const { return rotation_keys_size_; }

  inline double relin_keys_size() const { return relin_keys_size_; }

  inline double ctxt_inputs_size() const { return ctxt_inputs_size_; }

  inline double ctxt_outputs_size() const { return ctxt_outputs_size_; }

private:
  std::shared_ptr<ir::Func> func_;

  bool depth_metrics_ = false;

  DepthSummary depth_summary_{};

  CtxtTermsDepthInfo ctxt_leaves_depth_info_{};

  bool terms_classes_metrics_ = false;

  double terms_count_ = 0;

  double captured_terms_count_ = 0;

  double ctxt_inputs_count_ = 0;

  double ptxt_leaves_count_ = 0;

  double constants_count_ = 0;

  double pp_ops_count_ = 0;

  double constants_ops_count_ = 0;

  double circuit_static_cost_ = 0;

  CCOpsCounts cc_mul_counts_{};

  double cc_mul_total_ = 0;

  CAOpsCounts square_counts_{};

  double square_total_ = 0;

  double encrypt_count_ = 0;

  CAOpsCounts relin_counts_{};

  double relin_total_ = 0;

  CAOpsCounts rotate_counts_{};

  double rotate_total_ = 0;

  CAOpsCounts cp_mul_counts_{};

  double cp_mul_total_ = 0;

  CAOpsCounts c_scalar_mul_counts_{};

  double c_scalar_mul_total_ = 0;

  CAOpsCounts c_non_scalar_mul_counts_{};

  double c_non_scalar_mul_total_ = 0;

  CAOpsCounts mod_switch_counts_{};

  double mod_switch_total_ = 0;

  CAOpsCounts he_add_counts_{};

  double he_add_total_ = 0;

  CtxtTermsInfo ctxt_outputs_info_{};

  double relin_keys_count_ = 0;

  double rotation_keys_count_ = 0;

  bool global_metrics_ = false;

  double circuit_cost_ = 0;

  double rotation_keys_size_ = 0;

  double relin_keys_size_ = 0;

  double ctxt_inputs_size_ = 0;

  double ctxt_outputs_size_ = 0;

  friend Quantifier operator/(const Quantifier &lhs, const Quantifier &rhs);
  friend Quantifier operator-(const Quantifier &lhs, const Quantifier &rhs);
  friend Quantifier operator*(const Quantifier &lhs, int coeff);
};

Quantifier operator/(const Quantifier &lhs, const Quantifier &rhs);

Quantifier &operator/=(Quantifier &lhs, const Quantifier &rhs);

Quantifier operator-(const Quantifier &lhs, const Quantifier &rhs);

Quantifier &operator-=(Quantifier &lhs, const Quantifier &rhs);

Quantifier operator*(const Quantifier &lhs, int coeff);

Quantifier operator*(int coeff, const Quantifier &rhs);

Quantifier operator*=(Quantifier &lhs, int coeff);

Quantifier::DepthSummary operator/(const Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs);

Quantifier::DepthSummary &operator/=(Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs);

Quantifier::DepthSummary operator-(const Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs);

Quantifier::DepthSummary &operator-=(Quantifier::DepthSummary &lhs, const Quantifier::DepthSummary &rhs);

Quantifier::DepthSummary operator*(const Quantifier::DepthSummary &lhs, int coeff);

Quantifier::DepthSummary operator*(int coeff, const Quantifier::DepthSummary &rhs);

Quantifier::DepthSummary operator*=(Quantifier::DepthSummary &lhs, int coeff);

Quantifier::CtxtTermsDepthInfo operator/(
  const Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs);

Quantifier::CtxtTermsDepthInfo &operator/=(
  Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs);

Quantifier::CtxtTermsDepthInfo operator-(
  const Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs);

Quantifier::CtxtTermsDepthInfo &operator-=(
  Quantifier::CtxtTermsDepthInfo &lhs, const Quantifier::CtxtTermsDepthInfo &rhs);

Quantifier::CtxtTermsDepthInfo operator*(const Quantifier::CtxtTermsDepthInfo &lhs, int coeff);

Quantifier::CtxtTermsDepthInfo operator*(int coeff, const Quantifier::CtxtTermsDepthInfo &rhs);

Quantifier::CtxtTermsDepthInfo operator*=(Quantifier::CtxtTermsDepthInfo &lhs, int coeff);

Quantifier::DepthInfo operator/(const Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs);

Quantifier::DepthInfo &operator/=(Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs);

Quantifier::DepthInfo operator-(const Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs);

Quantifier::DepthInfo &operator-=(Quantifier::DepthInfo &lhs, const Quantifier::DepthInfo &rhs);

Quantifier::DepthInfo operator*(const Quantifier::DepthInfo &lhs, int coeff);

Quantifier::DepthInfo operator*(int coeff, const Quantifier::DepthInfo &rhs);

Quantifier::DepthInfo operator*=(Quantifier::DepthInfo &lhs, int coeff);

Quantifier::CCOpsCounts operator/(const Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs);

Quantifier::CCOpsCounts &operator/=(Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs);

Quantifier::CCOpsCounts operator-(const Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs);

Quantifier::CCOpsCounts &operator-=(Quantifier::CCOpsCounts &lhs, const Quantifier::CCOpsCounts &rhs);

Quantifier::CCOpsCounts operator*(const Quantifier::CCOpsCounts &lhs, int coeff);

Quantifier::CCOpsCounts operator*(int coeff, const Quantifier::CCOpsCounts &rhs);

Quantifier::CCOpsCounts operator*=(Quantifier::CCOpsCounts &lhs, int coeff);

Quantifier::CAOpsCounts operator/(const Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs);

Quantifier::CAOpsCounts &operator/=(Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs);

Quantifier::CAOpsCounts operator-(const Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs);

Quantifier::CAOpsCounts &operator-=(Quantifier::CAOpsCounts &lhs, const Quantifier::CAOpsCounts &rhs);

Quantifier::CAOpsCounts operator*(const Quantifier::CAOpsCounts &lhs, int coeff);

Quantifier::CAOpsCounts operator*(int coeff, const Quantifier::CAOpsCounts &rhs);

Quantifier::CAOpsCounts operator*=(Quantifier::CAOpsCounts &lhs, int coeff);

Quantifier::CtxtTermsInfo operator/(const Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs);

Quantifier::CtxtTermsInfo &operator/=(Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs);

Quantifier::CtxtTermsInfo operator-(const Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs);

Quantifier::CtxtTermsInfo &operator-=(Quantifier::CtxtTermsInfo &lhs, const Quantifier::CtxtTermsInfo &rhs);

Quantifier::CtxtTermsInfo operator*(const Quantifier::CtxtTermsInfo &lhs, int coeff);

Quantifier::CtxtTermsInfo operator*(int coeff, const Quantifier::CtxtTermsInfo &rhs);

Quantifier::CtxtTermsInfo operator*=(Quantifier::CtxtTermsInfo &lhs, int coeff);

Quantifier::CtxtTermInfo operator/(const Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs);

Quantifier::CtxtTermInfo &operator/=(Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs);

Quantifier::CtxtTermInfo operator-(const Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs);

Quantifier::CtxtTermInfo &operator-=(Quantifier::CtxtTermInfo &lhs, const Quantifier::CtxtTermInfo &rhs);

Quantifier::CtxtTermInfo operator*(const Quantifier::CtxtTermInfo &lhs, int coeff);

Quantifier::CtxtTermInfo operator*(int coeff, const Quantifier::CtxtTermInfo &rhs);

Quantifier::CtxtTermInfo operator*=(Quantifier::CtxtTermInfo &lhs, int coeff);

std::ostream &operator<<(std::ostream &os, const Quantifier &quantifier);

std::ostream &operator<<(std::ostream &os, const Quantifier::CtxtTermsDepthInfo &ctxt_terms_depth_info);

std::ostream &operator<<(std::ostream &os, const Quantifier::DepthInfo &ctxt_term_depth_info);

std::ostream &operator<<(std::ostream &os, const Quantifier::CCOpsCounts &cc_ops_counts);

std::ostream &operator<<(std::ostream &os, const Quantifier::CAOpsCounts &ca_op_counts);

std::ostream &operator<<(std::ostream &os, const Quantifier::CtxtTermsInfo &ctxt_terms_info);

std::ostream &operator<<(std::ostream &os, const Quantifier::CtxtTermInfo &ctxt_term_info);
} // namespace fheco::util
