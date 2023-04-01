#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace utils
{
class ClearDataEvaluator
{
public:
  ClearDataEvaluator() {}

  ClearDataEvaluator(std::size_t vector_size, int bit_width)
    : vector_size_(vector_size), max_value_((2 << (bit_width - 1)) - 1)
  {
    static_assert(sizeof(long long) * 8 >= 60, "computation max modulus must fit in long long");
  }

  template <typename T>
  inline std::vector<T> make_element(const std::vector<T> &v) const
  {
    constexpr bool t_is_int64_t = std::is_same_v<T, std::int64_t>;
    constexpr bool t_is_uint64_t = std::is_same_v<T, std::uint64_t>;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    std::vector<T> element(v);
    element.resize(vector_size_);
    return element;
  }

  template <typename T>
  inline std::vector<T> make_element(T value) const
  {
    constexpr bool t_is_int64_t = std::is_same_v<T, std::int64_t>;
    constexpr bool t_is_uint64_t = std::is_same_v<T, std::uint64_t>;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    std::vector<T> element(vector_size_, value);
    return element;
  }

  template <typename T>
  inline void validate(const std::vector<T> &arg) const
  {
    constexpr bool t_is_int64_t = std::is_same_v<T, std::int64_t>;
    constexpr bool t_is_uint64_t = std::is_same_v<T, std::uint64_t>;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    if (arg.size() != vector_size_)
      throw std::invalid_argument("invalid vector size");

    for (std::size_t i = 0; i < vector_size_; ++i)
    {
      if constexpr (t_is_int64_t)
      {
        if (arg[i] > max_value_ || arg[i] < -max_value_)
          throw std::invalid_argument("invalid vector slot value");
      }
      else
      {
        if (arg[i] > max_value_)
          throw std::invalid_argument("invalid vector slot value");
      }
    }
  }

  template <typename T>
  inline void validate(T arg) const
  {
    constexpr bool t_is_int64_t = std::is_same_v<T, std::int64_t>;
    constexpr bool t_is_uint64_t = std::is_same_v<T, std::uint64_t>;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    if constexpr (t_is_int64_t)
    {
      if (arg > max_value_ || arg < -max_value_)
        throw std::invalid_argument("invalid scalar value");
    }
    else
    {
      if (arg > max_value_)
        throw std::invalid_argument("invalid scalar value");
    }
  }

  template <typename T1, typename T2>
  inline std::vector<T1> add(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] + arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> add(const std::vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] + arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> add(T1 arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1 + arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline T1 add(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    T1 destination = arg1 + arg2;
    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> sub(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] - arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> sub(const std::vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and destination has the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] - arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> sub(T1 arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and destination has the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1 - arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline T1 sub(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    T1 destination = arg1 - arg2;
    validate(destination);
    return destination;
  }

  template <typename T>
  inline std::vector<T> negate(const std::vector<T> &arg) const
  {
    validate(arg);

    std::vector<T> destination(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = -arg[i];

    validate(destination);
    return destination;
  }

  template <typename T>
  inline T negate(T arg) const
  {
    validate(arg);
    T destination = -arg;
    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> mul(const std::vector<T1> &arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] * arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> mul(const std::vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] * arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline std::vector<T1> mul(T1 arg1, const std::vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    std::vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1 * arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  inline T1 mul(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = std::is_same_v<T1, std::uint64_t>;
    constexpr bool t2_is_uint64_t = std::is_same_v<T2, std::uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    T1 destination = arg1 * arg2;
    validate(destination);
    return destination;
  }

  template <typename T>
  inline std::vector<T> rotate(const std::vector<T> &arg, int steps) const
  {
    validate(arg);
    std::vector<T> destination(vector_size_);
    steps %= vector_size_;
    if (steps < 0)
      steps += vector_size_;
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg[(i + steps) % vector_size_];
    return destination;
  }

private:
  std::size_t vector_size_ = 0;
  long long max_value_ = 0;
};
} // namespace utils

namespace std
{
bool operator==(const std::vector<std::uint64_t> &lhs, const std::vector<std::int64_t> &rhs);

inline bool operator==(const std::vector<std::int64_t> &lhs, const std::vector<std::uint64_t> &rhs)
{
  return rhs == lhs;
}
} // namespace std
