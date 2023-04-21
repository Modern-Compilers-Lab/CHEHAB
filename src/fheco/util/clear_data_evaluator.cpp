#include "clear_data_evaluator.hpp"
#include <stdexcept>

using namespace std;

namespace fhecompiler
{
namespace util
{
  template <typename T>
  vector<T> ClearDataEvaluator::make_element(const vector<T> &vector_value) const
  {
    vector<T> element{vector_value};
    element.resize(vector_size_);
    validate(element);
    return element;
  }

  template <typename T>
  vector<T> ClearDataEvaluator::make_element(T scalar_value) const
  {
    vector<T> element(vector_size_, scalar_value);
    validate(element);
    return element;
  }

  template <typename T>
  void ClearDataEvaluator::validate(const vector<T> &arg) const
  {
    if (arg.size() != vector_size_)
      throw invalid_argument("invalid vector size");

    for (size_t i = 0; i < vector_size_; ++i)
    {
      if constexpr (t_is_int64_t)
      {
        if (arg[i] > max_value_ || arg[i] < -max_value_)
          throw invalid_argument("invalid vector slot value");
      }
      else
      {
        if (arg[i] > max_value_)
          throw invalid_argument("invalid vector slot value");
      }
    }
  }

  template <typename T>
  void ClearDataEvaluator::validate(T arg) const
  {
    constexpr bool t_is_int64_t = is_same_v<T, int64_t>;
    constexpr bool t_is_uint64_t = is_same_v<T, uint64_t>;
    static_assert(t_is_int64_t || t_is_uint64_t, "only int64_t and uint64_t are supported");

    if constexpr (t_is_int64_t)
    {
      if (arg > max_value_ || arg < -max_value_)
        throw invalid_argument("invalid scalar value");
    }
    else
    {
      if (arg > max_value_)
        throw invalid_argument("invalid scalar value");
    }
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::add(const vector<T1> &arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] + arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::add(const vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] + arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::add(T1 arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1 + arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  T1 ClearDataEvaluator::add(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
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
  vector<T1> ClearDataEvaluator::sub(const vector<T1> &arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] - arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::sub(const vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and destination has the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] - arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::sub(T1 arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and destination has the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = arg1 - arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  T1 ClearDataEvaluator::sub(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
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
  vector<T> ClearDataEvaluator::negate(const vector<T> &arg) const
  {
    validate(arg);

    vector<T> destination(vector_size_);
    for (size_t i = 0; i < vector_size_; ++i)
      destination[i] = -arg[i];

    validate(destination);
    return destination;
  }

  template <typename T>
  T ClearDataEvaluator::negate(T arg) const
  {
    validate(arg);
    T destination = -arg;
    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::mul(const vector<T1> &arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] * arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::mul(const vector<T1> &arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1[i] * arg2;

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  vector<T1> ClearDataEvaluator::mul(T1 arg1, const vector<T2> &arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
    constexpr bool to_uint64_t = t1_is_uint64_t || t2_is_uint64_t;
    static_assert(
      t1_is_uint64_t || !t2_is_uint64_t,
      "int64_t f int64_t -> int64_t otherwise uint64_t and result will have the same type as arg1");

    validate(arg1);
    validate(arg2);
    vector<T1> destination(vector_size_);
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg1 * arg2[i];

    validate(destination);
    return destination;
  }

  template <typename T1, typename T2>
  T1 ClearDataEvaluator::mul(T1 arg1, T2 arg2) const
  {
    constexpr bool t1_is_uint64_t = is_same_v<T1, uint64_t>;
    constexpr bool t2_is_uint64_t = is_same_v<T2, uint64_t>;
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
  vector<T> ClearDataEvaluator::rotate(const vector<T> &arg, int steps) const
  {
    validate(arg);
    vector<T> destination(vector_size_);
    steps %= vector_size_;
    if (steps < 0)
      steps += vector_size_;
    for (int i = 0; i < vector_size_; ++i)
      destination[i] = arg[(i + steps) % vector_size_];
    return destination;
  }
} // namespace util
} // namespace fhecompiler
