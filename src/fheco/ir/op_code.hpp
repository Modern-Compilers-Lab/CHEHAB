#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace fheco::ir
{
class OpCode 
{
public:
  enum class Type
  {
    nop,
    encrypt,
    add,
    sub,
    negate,
    rotate,
    square,
    mul,
    mod_switch,
    relin,
    SumVec,
  };

  static const OpCode nop; 
  static const OpCode encrypt;
  static const OpCode add;
  static const OpCode sub;
  static const OpCode negate;
  static OpCode rotate(int steps);
  static const OpCode square;
  static const OpCode mul;
  static const OpCode mod_switch;
  static const OpCode relin;
  ////****************************
  static OpCode SumVec(int size) ;
  ////****************************
  OpCode(Type type, std::vector<int> generators, std::size_t arity, bool commutativity, std::string str_repr)
    : type_{type}, generators_{std::move(generators)}, arity_{arity}, commutativity_{commutativity},
      str_repr_{std::move(str_repr)}
  {}

  inline Type type() const { return type_; }

  inline const std::vector<int> &generators() const { return generators_; }

  inline std::size_t arity() const { return arity_; }

  inline bool commutativity() const { return commutativity_; }

  inline const std::string &str_repr() const { return str_repr_; }

  // an alias of o.generators()[0] for rotate operation type
  int steps() const;
  int size() const;
private:
  Type type_;

  std::vector<int> generators_;

  std::size_t arity_;

  bool commutativity_;

  std::string str_repr_;
};

bool operator==(const OpCode &lhs, const OpCode &rhs);

bool operator<(const OpCode &lhs, const OpCode &rhs);

inline bool operator!=(const OpCode &lhs, const OpCode &rhs)
{
  return !(lhs == rhs);
}

inline bool operator>(const OpCode &lhs, const OpCode &rhs)
{
  return rhs < lhs;
}

inline bool operator<=(const OpCode &lhs, const OpCode &rhs)
{
  return !(lhs > rhs);
}

inline bool operator>=(const OpCode &lhs, const OpCode &rhs)
{
  return !(lhs < rhs);
}

inline std::ostream &operator<<(std::ostream &os, const OpCode &op_code)
{
  return os << op_code.str_repr();
}

std::ostream &operator<<(std::ostream &os, OpCode::Type op_code_type);
} // namespace fheco::ir

namespace std
{
template <>
struct hash<fheco::ir::OpCode>
{
  size_t operator()(const fheco::ir::OpCode &op_code) const;
};
} // namespace std
