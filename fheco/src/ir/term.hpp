#pragma once

#include "op_code.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <functional>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class Term
  {
  public:
    using Set = std::set<Term *, std::function<bool(Term *, Term *)>>;

    using USet = std::unordered_set<Term *, std::function<std::size_t(Term *)>, std::function<bool(Term *, Term *)>>;

    using UMultiSet =
      std::unordered_multiset<Term *, std::function<std::size_t(Term *)>, std::function<bool(Term *, Term *)>>;

    Term(std::string label, OpCode op_code, std::vector<Term *> operands);

    Term(std::string label, TermType type)
      : label_{std::move(label)}, op_code_{OpCode::nop}, operands_{}, type_{std::move(type)}
    {}

    friend inline bool operator==(const Term &lhs, const Term &rhs) { return lhs.label_ == rhs.label_; }

    friend inline bool operator<(const Term &lhs, const Term &rhs) { return lhs.label_ < rhs.label_; }

    inline const std::string &label() const { return label_; }

    inline const TermType &type() const { return type_; }

    inline const OpCode &op_code() const { return op_code_; }

    inline const std::vector<Term *> &operands() const { return operands_; }

    inline const UMultiSet &parents() const { return parents_; }

    inline bool is_operation() const { return op_code_ != OpCode::nop; }

  private:
    void replace_with(Term *t);

    std::string label_;

    OpCode op_code_;

    std::vector<Term *> operands_;

    TermType type_;

    static inline std::size_t hash(Term *t) { return std::hash<std::string>{}(t->label_); }

    static inline bool equal(Term *lhs, Term *rhs) { return *lhs == *rhs; }

    static inline bool less(Term *lhs, Term *rhs) { return *lhs < *rhs; }

    UMultiSet parents_{0, hash, equal};

    friend class DAG;
  };

  inline bool operator!=(const Term &lhs, const Term &rhs)
  {
    return !(lhs == rhs);
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
} // namespace ir
} // namespace fhecompiler
