#pragma once

#include "term_type.hpp"
#include <cstddef>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class Term;

  class OpCode
  {
  public:
    static const OpCode nop;
    static const OpCode encrypt;
    static const OpCode add;
    static const OpCode sub;
    static const OpCode negate;
    static const std::function<OpCode(int)> rotate;
    static const OpCode square;
    static const OpCode mul;
    static const OpCode mod_switch;
    static const OpCode relin;

    static const TermType &deduce_result_type(const OpCode &op_code, const std::vector<Term *> &operands);

    friend bool operator==(const OpCode &lhs, const OpCode &rhs);

    friend bool operator<(const OpCode &lhs, const OpCode &rhs);

    inline explicit operator int() const { return index_; }

    inline const std::vector<int> &generators() const { return generators_; }

    inline std::size_t arity() const { return arity_; }

    inline bool commutativity() const { return commutativity_; }

    inline const std::string &str_repr() const { return str_repr_; }

    friend inline std::ostream &operator<<(std::ostream &os, const OpCode &op_code) { return os << op_code.str_repr_; }

    // an alias of o.generators()[0] for the rotate operation
    inline int steps() const
    {
      if (index_ != rotate(0).index_)
        throw std::invalid_argument("steps should be called only on rotate_* operations");

      return generators_[0];
    }

  private:
    OpCode(int index, std::vector<int> generators, std::size_t arity, bool commutativity, std::string str_repr)
      : index_{index}, generators_{std::move(generators)}, arity_{arity},
        commutativity_{commutativity}, str_repr_{std::move(str_repr)}
    {}

    OpCode(std::vector<int> generators, std::size_t arity, bool commutativity, std::string str_repr)
      : OpCode(count_++, std::move(generators), arity, commutativity, std::move(str_repr))
    {}

    static int count_;

    int index_;

    std::vector<int> generators_;

    std::size_t arity_;

    bool commutativity_;

    std::string str_repr_;
  };

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
} // namespace ir
} // namespace fhecompiler

namespace std
{
template <>
struct hash<fhecompiler::ir::OpCode>
{
  size_t operator()(const fhecompiler::ir::OpCode &e) const { return hash<int>()(static_cast<int>(e)); }
};
} // namespace std
