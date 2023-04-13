#pragma once

#include "op_code.hpp"
#include "term_type.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class Term
  {
  public:
    Term(std::string label, OpCode op_code, std::vector<Term *> operands)
      : label_{std::move(label)}, op_code_{std::move(op_code)}, operands_{std::move(operands)},
        type_{OpCode::deduce_result_type(op_code_, operands_)}
    {}

    Term(std::string label, TermType type)
      : label_{std::move(label)}, op_code_{OpCode::nop}, operands_{}, type_{std::move(type)}
    {}

    friend inline bool operator==(const Term &lhs, const Term &rhs) { return lhs.label_ == rhs.label_; }

    friend inline bool operator<(const Term &lhs, const Term &rhs) { return lhs.label_ < rhs.label_; }

    inline const std::string &label() const { return label_; }

    inline const TermType &type() const { return type_; }

    inline const OpCode &op_code() const { return op_code_; }

    inline const std::vector<Term *> &operands() const { return operands_; }

    inline const std::unordered_multimap<std::string, std::shared_ptr<Term>> &parents() const { return parents_; }

    inline bool is_operation() const { return op_code_ != OpCode::nop; }

  private:
    inline void add_parent(std::shared_ptr<Term> parent) { parents_.emplace(parent->label_, std::move(parent)); }

    inline void delete_parent(const std::string &label)
    {
      auto it = parents_.find(label);
      if (it != parents_.end())
        parents_.erase(it);
    }

    void replace_with(Term *t);

    std::string label_;

    OpCode op_code_;

    std::vector<Term *> operands_;

    TermType type_;

    std::unordered_multimap<std::string, std::shared_ptr<Term>> parents_;

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
