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

    friend inline bool operator==(const TermType &lhs, const TermType &rhs) { return lhs.index_ == rhs.index_; }

    friend inline bool operator<(const TermType &lhs, const TermType &rhs) { return lhs.index_ < rhs.index_; }

    inline explicit operator int() const { return index_; }

    inline const std::string &auto_name_prefix() const { return auto_name_prefix_; }

    friend inline std::ostream &operator<<(std::ostream &os, const TermType &term_type)
    {
      return os << term_type.auto_name_prefix_;
    }

  private:
    TermType(std::string auto_name_prefix) : index_{count_++}, auto_name_prefix_{std::move(auto_name_prefix)} {}

    static int count_;

    int index_;

    std::string auto_name_prefix_;
  };

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
} // namespace ir
} // namespace fhecompiler

namespace std
{
template <>
struct hash<fhecompiler::ir::TermType>
{
  size_t operator()(const fhecompiler::ir::TermType &e) const { return hash<int>()(static_cast<int>(e)); }
};
} // namespace std
