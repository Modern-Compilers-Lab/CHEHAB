#pragma once

#include <ostream>
#include <string>
#include <utility>

namespace fhecompiler
{
namespace ir
{
  class TermType
  {
  public:
    static const TermType ciphertext;
    static const TermType plaintext;
    static const TermType scalar;

    inline explicit operator int() const { return index_; }

    inline int index() const { return index_; }

    inline const std::string &str_repr() const { return str_repr_; }

  private:
    TermType(std::string str_repr_) : index_{count_++}, str_repr_{std::move(str_repr_)} {}

    static int count_;

    int index_;

    std::string str_repr_;
  };

  inline bool operator==(const TermType &lhs, const TermType &rhs)
  {
    return lhs.index() == rhs.index();
  }

  inline bool operator<(const TermType &lhs, const TermType &rhs)
  {
    return lhs.index() < rhs.index();
  }

  inline bool operator!=(const TermType &lhs, const TermType &rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator>(const TermType &lhs, const TermType &rhs)
  {
    return rhs < lhs;
  }

  inline bool operator<=(const TermType &lhs, const TermType &rhs)
  {
    return !(lhs > rhs);
  }

  inline bool operator>=(const TermType &lhs, const TermType &rhs)
  {
    return !(lhs < rhs);
  }

  inline std::ostream &operator<<(std::ostream &os, const TermType &term_type)
  {
    return os << term_type.str_repr();
  }
} // namespace ir
} // namespace fhecompiler

namespace std
{
template <>
struct hash<fhecompiler::ir::TermType>
{
  inline size_t operator()(const fhecompiler::ir::TermType &term_type) const { return hash<int>()(term_type.index()); }
};
} // namespace std
