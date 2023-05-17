#pragma once

#include "fheco/trs/common.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace fheco::trs
{
class TermMatcher
{
public:
  static TermMatcherType deduce_result_type(const TermOpCode &op_code, const std::vector<TermMatcher> &operands);

  TermMatcher(TermOpCode op_code, std::vector<TermMatcher> operands)
    : id_{++count_}, op_code_{std::move(op_code)}, operands_{std::move(operands)},
      type_{deduce_result_type(op_code_, operands_)}, label_{}, val_{}
  {}

  TermMatcher(TermMatcherType type, std::string label)
    : id_{++count_}, op_code_{TermOpCode::nop}, operands_{}, type_{type}, label_{std::move(label)}, val_{}
  {}

  TermMatcher(PackedVal val)
    : id_{++count_}, op_code_{TermOpCode::nop}, operands_{}, type_{TermMatcherType::const_}, label_{},
      val_{std::move(val)}
  {}

  TermMatcher(std::string label, PackedVal val)
    : id_{++count_}, op_code_{TermOpCode::nop}, operands_{}, type_{TermMatcherType::const_}, label_{std::move(label)},
      val_{std::move(val)}
  {}

  inline std::size_t id() const { return id_; }

  inline const TermOpCode &op_code() const { return op_code_; };

  inline const std::vector<TermMatcher> &operands() const { return operands_; }

  inline TermMatcherType type() const { return type_; }

  inline const std::optional<std::string> &label() const { return label_; };

  inline const std::optional<PackedVal> &val() const { return val_; };

  inline bool is_variable() const { return operands_.empty() && !val_; }

private:
  static std::size_t count_;

  std::size_t id_;

  TermOpCode op_code_;

  std::vector<TermMatcher> operands_;

  TermMatcherType type_;

  std::optional<std::string> label_;

  std::optional<PackedVal> val_;
};

inline bool operator==(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return lhs.id() == rhs.id();
}

inline bool operator<(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return lhs.id() < rhs.id();
}

inline bool operator!=(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return !(lhs == rhs);
}

inline bool operator>(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return rhs < lhs;
}

inline bool operator<=(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return !(lhs > rhs);
}

inline bool operator>=(const TermMatcher &lhs, const TermMatcher &rhs)
{
  return !(lhs < rhs);
}
} // namespace fheco::trs

namespace std
{
template <>
struct hash<fheco::trs::TermMatcher>
{
  inline size_t operator()(const fheco::trs::TermMatcher &term_matcher) const
  {
    return hash<size_t>()(term_matcher.id());
  }
};
} // namespace std
