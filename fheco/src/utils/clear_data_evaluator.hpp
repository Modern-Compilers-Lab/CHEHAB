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

  ClearDataEvaluator(std::size_t vector_size, int bit_width, bool signedness)
    : vector_size_(vector_size), max_value_((2 << (bit_width - 1)) - 1), signedness_(signedness)
  {
    static_assert(sizeof(long long) * 8 >= 60, "computation max modulus must fit in long long");
  }

  template <typename T>
  inline std::vector<T> make_element(const std::vector<T> &v)
  {
    constexpr bool t_is_int64_t = std::is_same<T, std::int64_t>::value;
    constexpr bool t_is_uint64_t = std::is_same<T, std::uint64_t>::value;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    std::vector<T> element(v);
    element.resize(vector_size_);
    return element;
  }

  template <typename T>
  inline std::vector<T> make_element(T value)
  {
    constexpr bool t_is_int64_t = std::is_same<T, std::int64_t>::value;
    constexpr bool t_is_uint64_t = std::is_same<T, std::uint64_t>::value;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    std::vector<T> element(vector_size_, value);
    return element;
  }

  template <typename T>
  inline void validate(const std::vector<T> &arg)
  {
    constexpr bool t_is_int64_t = std::is_same<T, std::int64_t>::value;
    constexpr bool t_is_uint64_t = std::is_same<T, std::uint64_t>::value;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    if (arg.size() != vector_size_)
      throw std::invalid_argument("invalid vector size");

    for (std::size_t i = 0; i < vector_size_; ++i)
    {
      if (t_is_int64_t)
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

  template <typename T1, typename T2, typename T3>
  inline void add(const std::vector<T1> &arg1, const std::vector<T2> &arg2, std::vector<T3> &destination)
  {
    constexpr bool t1_is_uint64_t = std::is_same<T1, std::uint64_t>::value;
    constexpr bool t2_is_uint64_t = std::is_same<T2, std::uint64_t>::value;
    constexpr bool t3_is_uint64_t = std::is_same<T3, std::uint64_t>::value;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      (t3_is_uint64_t && to_uint64_t) || (!t3_is_uint64_t && !to_uint64_t),
      "int64_t f int64_t -> int64_t otherwise uint64_t");

    validate(arg1);
    validate(arg2);
    destination.resize(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] + arg2[i];

    validate(destination);
  }

  template <typename T1, typename T2, typename T3>
  inline void sub(const std::vector<T1> &arg1, const std::vector<T2> &arg2, std::vector<T3> &destination)
  {
    constexpr bool t1_is_uint64_t = std::is_same<T1, std::uint64_t>::value;
    constexpr bool t2_is_uint64_t = std::is_same<T2, std::uint64_t>::value;
    constexpr bool t3_is_uint64_t = std::is_same<T3, std::uint64_t>::value;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      (t3_is_uint64_t && to_uint64_t) || (!t3_is_uint64_t && !to_uint64_t),
      "int64_t f int64_t -> int64_t otherwise uint64_t");

    validate(arg1);
    validate(arg2);
    destination.resize(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] - arg2[i];

    validate(destination);
  }

  template <typename T>
  inline void negate(const std::vector<T> &arg, std::vector<T> &destination)
  {
    validate(arg);
    destination.resize(vector_size_);
    for (std::size_t i = 0; i < vector_size_; ++i)
      destination[i] = -arg[i];

    validate(destination);
  }

  template <typename T1, typename T2, typename T3>
  void multiply(const std::vector<T1> &arg1, const std::vector<T2> &arg2, std::vector<T3> &destination)
  {
    constexpr bool t1_is_uint64_t = std::is_same<T1, std::uint64_t>::value;
    constexpr bool t2_is_uint64_t = std::is_same<T2, std::uint64_t>::value;
    constexpr bool t3_is_uint64_t = std::is_same<T3, std::uint64_t>::value;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      (t3_is_uint64_t && to_uint64_t) || (!t3_is_uint64_t && !to_uint64_t),
      "int64_t f int64_t -> int64_t otherwise uint64_t");

    validate(arg1);
    validate(arg2);
    destination.resize(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] * arg2[i];

    validate(destination);
  }

  template <typename T>
  inline void rotate(const std::vector<T> &arg, int step, std::vector<T> &destination)
  {
    validate(arg);
    destination.resize(vector_size_);
    std::vector<T> copy(arg);
    step %= vector_size_;
    if (step < 0)
      step += vector_size_;
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = copy[(i + step) % vector_size_];
  }

private:
  std::size_t vector_size_ = 0;
  bool signedness_ = true;
  long long max_value_ = 0;
};

} // namespace utils
