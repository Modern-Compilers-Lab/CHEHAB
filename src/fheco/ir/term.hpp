#pragma once

#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <ostream> 
#include <string>
#include <unordered_set>
#include <vector>

namespace fheco::ir
{
class Term
{
public:
  struct HashPtr
  {
    std::size_t operator()(const Term *p) const;
  };

  struct EqualPtr
  {
    bool operator()(const Term *lhs, const Term *rhs) const;
  };

  struct ComparePtr
  {
    bool operator()(const Term *lhs, const Term *rhs) const;
  };

  using PtrSet = std::unordered_set<Term *, HashPtr, EqualPtr>;

  // order of definition is important for type deduction (OpCode::deduce_result_type)
  enum class Type
  {
    cipher,
    plain,
    undefined
  };

  static std::string term_type_str_repr(Type);

  static Type deduce_result_type(const OpCode &op_code, const std::vector<Term *> &operands);

  static Type deduce_result_type(const OpCode &op_code, const std::vector<Type> &operands_types);

  inline std::size_t id() const { return id_; }

  inline const OpCode &op_code() const { return op_code_; }

  inline const std::vector<Term *> &operands() const { return operands_; }

  inline Type type() const { return type_; }

  inline const PtrSet &parents() const { return parents_; }

  inline bool is_operation() const { return op_code_.type() != OpCode::Type::nop; }

  inline bool is_leaf() const { return operands_.empty(); }

  inline bool is_source() const { return parents_.empty(); }

private:
  Term(OpCode op_code, std::vector<Term *> operands);

  Term(Type type) : id_{++count_}, op_code_{OpCode::nop}, operands_{}, type_{type} {}

  // to construct temp object used as search keys
  Term(std::size_t id) : id_{id}, op_code_{OpCode::nop}, operands_{}, type_{Type::cipher} {}

  static std::size_t count_;

  std::size_t id_;

  OpCode op_code_;

  std::vector<Term *> operands_;

  Type type_;

  // it seems we don't need parent multiplicity
  PtrSet parents_{};

  friend class Expr;
};

inline bool operator==(const Term &lhs, const Term &rhs)
{
  return lhs.id() == rhs.id();
}

inline bool operator!=(const Term &lhs, const Term &rhs)
{
  return !(lhs == rhs);
}

inline bool operator<(const Term &lhs, const Term &rhs)
{
  return lhs.id() < rhs.id();
}

inline bool operator>(const Term &lhs, const Term &rhs)
{
  return rhs < lhs;
}

inline bool operator<=(const Term &lhs, const Term &rhs)
{
  return !(lhs > rhs);
}

inline bool operator>=(const Term &lhs, const Term &rhs)
{
  return !(lhs < rhs);
}

std::ostream &operator<<(std::ostream &os, Term::Type term_type);
} // namespace fheco::ir

namespace std
{
template <>
struct hash<fheco::ir::Term>
{
  inline size_t operator()(const fheco::ir::Term &term) const { return hash<size_t>()(term.id()); }
};
} // namespace std
