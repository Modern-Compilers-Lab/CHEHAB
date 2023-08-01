#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fheco::ir
{
class Func;

struct HashPackedVal
{
  std::size_t operator()(const PackedVal &packed_val) const;
};

struct OpType
{
  ir::OpCode::Type op_code_type_;
  std::vector<ir::Term::Type> operands_types_;
};

struct HashOpType
{
  std::size_t operator()(const OpType &op_type) const;
};

struct EqualOpType
{
  bool operator()(const OpType &lhs, const OpType &rhs) const;
};

enum class TermQualif
{
  in,
  in_out,
  const_,
  const_out,
  op,
  op_out
};

struct InputTermInfo
{
  std::string label_;
  std::optional<PackedVal> example_val_;
};

struct OutputTermInfo
{
  std::unordered_set<std::string> labels_;
  std::optional<PackedVal> example_val_;
};

inline bool operator==(const InputTermInfo &lhs, const InputTermInfo &rhs)
{
  return lhs.label_ == rhs.label_ && lhs.example_val_ == rhs.example_val_;
}

inline bool operator!=(const InputTermInfo &lhs, const InputTermInfo &rhs)
{
  return !(lhs == rhs);
}

inline bool operator==(const OutputTermInfo &lhs, const OutputTermInfo &rhs)
{
  return lhs.labels_ == rhs.labels_ && lhs.example_val_ == rhs.example_val_;
}

inline bool operator!=(const OutputTermInfo &lhs, const OutputTermInfo &rhs)
{
  return !(lhs == rhs);
}

using InputTermsInfo = std::unordered_map<const Term *, InputTermInfo, Term::HashPtr, Term::EqualPtr>;

using OutputTermsInfo = std::unordered_map<const Term *, OutputTermInfo, Term::HashPtr, Term::EqualPtr>;

struct ConstInfo
{
  bool is_scalar_;
  PackedVal val_;
};

inline bool operator==(const ConstInfo &lhs, const ConstInfo &rhs)
{
  return lhs.is_scalar_ == rhs.is_scalar_ && lhs.val_ == rhs.val_;
}

inline bool operator!=(const ConstInfo &lhs, const ConstInfo &rhs)
{
  return !(lhs == rhs);
}

using ConstTermsValues = std::unordered_map<const Term *, ConstInfo, Term::HashPtr, Term::EqualPtr>;

using TermsValues = std::unordered_map<const Term *, PackedVal, Term::HashPtr, Term::EqualPtr>;

using IOValues = std::unordered_map<std::string, PackedVal>;

IOValues convert_to_io_values(const InputTermsInfo &input_terms_info);

IOValues convert_to_io_values(const OutputTermsInfo &output_terms_info);

struct TermInfo
{
  Term::Type type_;
  bool is_const_;
  bool is_scalar_;
};

double static_eval_op(const std::shared_ptr<Func> &func, const OpCode &op_code, const std::vector<Term *> &operands);

double static_eval_op(const OpCode &op_code, const std::vector<TermInfo> &operands_info);
} // namespace fheco::ir
