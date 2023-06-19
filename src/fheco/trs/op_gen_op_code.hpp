#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

namespace fheco::trs
{
class OpGenOpCode
{
public:
  static const OpGenOpCode nop;
  static const OpGenOpCode add;
  static const OpGenOpCode sub;
  static const OpGenOpCode negate;
  static const OpGenOpCode mod;

  enum class Type
  {
    nop,
    add,
    sub,
    negate,
    mod
  };

  inline Type type() const { return type_; }

  inline std::size_t arity() const { return arity_; }

  inline const std::string &str_repr() const { return str_repr_; }

private:
  OpGenOpCode(Type type, std::size_t arity, std::string str_repr)
    : type_{type}, arity_{arity}, str_repr_{std::move(str_repr)}
  {}

  Type type_;

  std::size_t arity_;

  std::string str_repr_;
};

inline std::ostream &operator<<(std::ostream &os, const OpGenOpCode &op_code)
{
  return os << op_code.str_repr();
}

std::ostream &operator<<(std::ostream &os, OpGenOpCode::Type op_code_type);
} // namespace fheco::trs
