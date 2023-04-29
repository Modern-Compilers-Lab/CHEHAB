#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/function.hpp"
#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <utility>

namespace fheco::util
{
class Quantifier
{
public:
  // ctxt ctxt operation info
  struct CCOpInfo
  {
    std::size_t opposite_level_;
    std::size_t size1_;
    std::size_t size2_;
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
    std::size_t opposite_level_;
    std::size_t size_;
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

  struct DepthSummary
  {
    int min_xdepth_;
    int min_depth_;
    double avg_xdepth_;
    double avg_depth_;
    int max_xdepth_;
    int max_depth_;
  };

  struct DepthInfo
  {
    int xdepth_;
    int depth_;
  };

  using TermDepthInfo = std::unordered_map<std::size_t, DepthInfo>;

  Quantifier(std::shared_ptr<ir::Function> func) : func_{std::move(func)} {}

  void run_analysis();

  void compute_he_depth_info();

  void count_terms_classes();

  Quantifier operator-(const Quantifier &other) const;

  Quantifier &operator-=(const Quantifier &other);

  void print_info(std::ostream &os);

  void print_he_depth_info(std::ostream &os) const;

  void print_terms_classes_info(std::ostream &os) const;

  inline const std::shared_ptr<ir::Function> func() const { return func_; }

  inline const DepthSummary &he_depth_summary() const { return he_depth_summary_; }

  inline const TermDepthInfo &ctxt_leaves_depth_info() const { return ctxt_leaves_depth_info_; }

  inline std::int64_t all_terms_count() const { return all_terms_count_; }

  inline std::int64_t captured_terms_count() const { return captured_terms_count_; }

  inline std::int64_t ptxt_leaves_count() const { return ptxt_leaves_count_; }

  inline std::int64_t pp_ops_count() const { return pp_ops_count_; }

  inline std::int64_t relin_keys_count() const { return relin_keys_count_; }

  inline std::int64_t rotation_keys_count() const { return rotation_keys_count_; }

  inline std::int64_t ctxt_leaves_count() const { return ctxt_leaves_count_; }

  inline const CCOpCount &cc_mul_count() const { return cc_mul_count_; }

  inline const CAOpCount &he_square_count() const { return he_square_count_; }

  inline std::int64_t encrypt_count() const { return encrypt_count_; }

  inline const CAOpCount &relin_count() const { return relin_count_; }

  inline const CAOpCount &he_rotate_count() const { return he_rotate_count_; }

  inline const CAOpCount &cp_mul_count() const { return cp_mul_count_; }

  inline const CAOpCount &mod_switch_count() const { return mod_switch_count_; }

  inline const CAOpCount &he_add_sub_negate_count() const { return he_add_sub_negate_count_; }

private:
  std::shared_ptr<ir::Function> func_;

  DepthSummary he_depth_summary_{};

  TermDepthInfo ctxt_leaves_depth_info_{};

  std::int64_t all_terms_count_ = 0;

  std::int64_t captured_terms_count_ = 0;

  std::int64_t ptxt_leaves_count_ = 0;

  std::int64_t pp_ops_count_ = 0;

  std::int64_t relin_keys_count_ = 0;

  std::int64_t rotation_keys_count_ = 0;

  std::int64_t ctxt_leaves_count_ = 0;

  CCOpCount cc_mul_count_{};

  CAOpCount he_square_count_{};

  std::int64_t encrypt_count_ = 0;

  CAOpCount relin_count_{};

  CAOpCount he_rotate_count_{};

  CAOpCount cp_mul_count_{};

  CAOpCount mod_switch_count_{};

  CAOpCount he_add_sub_negate_count_{};
};
} // namespace fheco::util

namespace std
{
fheco::util::Quantifier::CCOpCount operator-(
  const fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs);

fheco::util::Quantifier::CCOpCount &operator-=(
  fheco::util::Quantifier::CCOpCount &lhs, const fheco::util::Quantifier::CCOpCount &rhs);

fheco::util::Quantifier::CAOpCount operator-(
  const fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs);

fheco::util::Quantifier::CAOpCount &operator-=(
  fheco::util::Quantifier::CAOpCount &lhs, const fheco::util::Quantifier::CAOpCount &rhs);

fheco::util::Quantifier::DepthSummary operator-(
  const fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs);

fheco::util::Quantifier::DepthSummary &operator-=(
  fheco::util::Quantifier::DepthSummary &lhs, const fheco::util::Quantifier::DepthSummary &rhs);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CCOpCount &cc_op_count);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::CAOpCount &ca_op_count);

ostream &operator<<(ostream &os, const fheco::util::Quantifier::TermDepthInfo &terms_depth_info);
} // namespace std
