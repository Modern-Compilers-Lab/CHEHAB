#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>

namespace fheco::ir
{
template <class>
inline constexpr bool always_false_v = false;

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

using IOTermsInfo = std::unordered_map<Term *, ParamTermInfo, Term::HashPtr, Term::EqualPtr>;

using TermsValues = std::unordered_map<Term *, PackedVal, Term::HashPtr, Term::EqualPtr>;
} // namespace fheco::ir
