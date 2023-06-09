#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace fheco::ir
{
template <class>
inline constexpr bool always_false_v = false;

std::int64_t evaluate_raw_op_code(const OpCode &op_code, const std::vector<Term *> &operands);

std::int64_t evaluate_raw_op_code(const OpCode &op_code, const std::vector<Term::Type> &operands_types);

// https://github.com/HowardHinnant/hash_append/issues/7#issuecomment-629414712
template <typename T>
inline void hash_combine(std::size_t &seed, const T &val)
{
  seed ^= std::hash<T>{}(val) + 0x9e3779b97f4a7c15ULL + (seed << 12) + (seed >> 4);
}

struct HashPackedVal
{
  std::size_t operator()(const PackedVal &packed_val) const;
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

struct ParamTermInfo
{
  std::string label_;
  std::optional<PackedVal> example_val_;
};

inline bool operator==(const ParamTermInfo &lhs, const ParamTermInfo &rhs)
{
  return lhs.label_ == rhs.label_ && lhs.example_val_ == rhs.example_val_;
}

inline bool operator!=(const ParamTermInfo &lhs, const ParamTermInfo &rhs)
{
  return !(lhs == rhs);
}

using IOTermsInfo = std::unordered_map<const Term *, ParamTermInfo, Term::HashPtr, Term::EqualPtr>;

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
} // namespace fheco::ir
