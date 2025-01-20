#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/op_gen_matcher.hpp"
#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace fheco::trs
{
/*
  ir::OpCode::Type type_;

  std::vector<OpGenMatcher> generators_;

  std::size_t arity_;

  bool commutativity_;

  std::string str_repr_;

  const TermOpCode TermOpCode::nop{ir::OpCode::Type::nop, {}, 0, false, "_"};
*/
class TermOpCode
{
public:
  static const TermOpCode nop;
  static const TermOpCode add;
  static const TermOpCode sub;
  static const TermOpCode negate; 
  static TermOpCode rotate(OpGenMatcher steps);
  static TermOpCode SumVec(OpGenMatcher steps);
  static const TermOpCode square;
  static const TermOpCode mul;

  static const TermOpCode relin;

  inline ir::OpCode::Type type() const { return type_; }

  inline const std::vector<OpGenMatcher> &generators() const { return generators_; }

  inline std::size_t arity() const { return arity_; }

  inline bool commutativity() const { return commutativity_; }

  inline const std::string &str_repr() const { return str_repr_; }

  // an alias of o.generators()[0] for rotate operation type
  const OpGenMatcher &steps() const;

private:
  TermOpCode(
    ir::OpCode::Type type, std::vector<OpGenMatcher> generators, std::size_t arity, bool commutativity,
    std::string str_repr)
    : type_{type}, generators_{std::move(generators)}, arity_{arity}, commutativity_{commutativity},
      str_repr_{std::move(str_repr)}
  {}

  ir::OpCode::Type type_;

  std::vector<OpGenMatcher> generators_;

  std::size_t arity_;

  bool commutativity_;

  std::string str_repr_;
};

inline std::ostream &operator<<(std::ostream &os, const TermOpCode &op_code)
{
  return os << op_code.str_repr();
}
} // namespace fheco::trs
