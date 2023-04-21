#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace fhecompiler
{
namespace util
{
  class ClearDataEvaluator
  {
  public:
    ClearDataEvaluator(std::size_t vector_size, int bit_width, bool signedness) : vector_size_{vector_size}
    {
      static_assert(sizeof(std::int64_t) * 8 > 60, "computation max modulus assumed to fit in int64_t");
      if (signedness)
        // 2^(bit_width-1) - 1
        max_value_ = (2 << (bit_width - 2)) - 1;
      else
        // 2^bit_width - 1
        max_value_ = (2 << (bit_width - 1)) - 1;
    }

    template <typename T>
    std::vector<T> make_element(const std::vector<T> &vector_value) const;

    template <typename T>
    std::vector<T> make_element(T scalar_value) const;

    template <typename T>
    void validate(const std::vector<T> &arg) const;

    template <typename T>
    void validate(T arg) const;

    template <typename T1, typename T2>
    std::vector<T1> add(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> add(const std::vector<T1> &arg1, T2 arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> add(T1 arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    T1 add(T1 arg1, T2 arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> sub(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> sub(const std::vector<T1> &arg1, T2 arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> sub(T1 arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    T1 sub(T1 arg1, T2 arg2) const;

    template <typename T>
    std::vector<T> negate(const std::vector<T> &arg) const;

    template <typename T>
    T negate(T arg) const;

    template <typename T1, typename T2>
    std::vector<T1> mul(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> mul(const std::vector<T1> &arg1, T2 arg2) const;

    template <typename T1, typename T2>
    std::vector<T1> mul(T1 arg1, const std::vector<T2> &arg2) const;

    template <typename T1, typename T2>
    T1 mul(T1 arg1, T2 arg2) const;

    template <typename T>
    std::vector<T> rotate(const std::vector<T> &arg, int steps) const;

  private:
    std::size_t vector_size_;
    std::int64_t max_value_;
  };
} // namespace util
} // namespace fhecompiler

namespace std
{
inline bool operator==(const std::vector<std::uint64_t> &lhs, const std::vector<std::int64_t> &rhs)
{
  return false;
}

inline bool operator==(const std::vector<std::int64_t> &lhs, const std::vector<std::uint64_t> &rhs)
{
  return rhs == lhs;
}
} // namespace std
