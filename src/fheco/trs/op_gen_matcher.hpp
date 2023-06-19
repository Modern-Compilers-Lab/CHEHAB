#pragma once

#include "fheco/trs/op_gen_op_code.hpp"
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace fheco::trs
{
class OpCode;

class OpGenMatcher
{
public:
  using RefWrapp = std::reference_wrapper<const OpGenMatcher>;

  struct HashRefWrapp
  {
    std::size_t operator()(const RefWrapp &matcher_ref) const;
  };

  struct EqualrRefWrapp
  {
    bool operator()(const RefWrapp &lhs, const RefWrapp &rhs) const;
  };

  OpGenMatcher(OpGenOpCode op_code, std::vector<OpGenMatcher> operands)
    : id_{++count_}, op_code_{std::move(op_code)}, operands_{std::move(operands)}, label_{}, val_{}
  {}

  OpGenMatcher(std::string label)
    : id_{++count_}, op_code_{OpGenOpCode::nop}, operands_{}, label_{std::move(label)}, val_{}
  {}

  OpGenMatcher(int val) : id_{++count_}, op_code_{OpGenOpCode::nop}, operands_{}, label_{}, val_{val} {}

  OpGenMatcher(std::string label, int val)
    : id_{++count_}, op_code_{OpGenOpCode::nop}, operands_{}, label_{std::move(label)}, val_{val}
  {}

  OpGenMatcher() : id_{0}, op_code_{OpGenOpCode::nop}, operands_{}, label_{}, val_{} {}

  inline std::size_t id() const { return id_; }

  inline const OpGenOpCode &op_code() const { return op_code_; };

  inline const std::vector<OpGenMatcher> &operands() const { return operands_; }

  inline const std::optional<std::string> &label() const { return label_; };

  inline const std::optional<int> &val() const { return val_; };

  inline bool is_operation() const { return op_code_.type() != OpGenOpCode::Type::nop; }

  inline bool is_leaf() const { return operands_.empty(); }

  inline bool is_variable() const { return operands_.empty() && !val_; }

private:
  static std::size_t count_;

  std::size_t id_;

  OpGenOpCode op_code_;

  std::vector<OpGenMatcher> operands_;

  std::optional<std::string> label_;

  std::optional<int> val_;
};

inline bool operator==(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return lhs.id() == rhs.id();
}

inline bool operator<(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return lhs.id() < rhs.id();
}

inline bool operator!=(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return !(lhs == rhs);
}

inline bool operator>(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return rhs < lhs;
}

inline bool operator<=(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return !(lhs > rhs);
}

inline bool operator>=(const OpGenMatcher &lhs, const OpGenMatcher &rhs)
{
  return !(lhs < rhs);
}
} // namespace fheco::trs

namespace std
{
template <>
struct hash<fheco::trs::OpGenMatcher>
{
  inline size_t operator()(const fheco::trs::OpGenMatcher &op_gen_matcher) const
  {
    return hash<size_t>()(op_gen_matcher.id());
  }
};
} // namespace std
