#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <bitset>
#include <cstddef>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

static_assert(sizeof(std::size_t) == 8, "require sizeof(std::size_t) == 8");

// from SEAL
#define FHECO_MSB_INDEX_UINT64(result, value)                            \
  {                                                                      \
    *result = 63UL - static_cast<unsigned long>(__builtin_clzll(value)); \
  }

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
inline bool is_power_of_two(std::size_t v)
{
  return v != 0 && (v & (v - 1)) == 0;
}

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
inline std::size_t next_power_of_two(std::size_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v;
}

// from SEAL
/**
if the value is a power of two, return the power; otherwise, return -1.
*/
inline int get_power_of_two(std::size_t value)
{
  if (value == 0 || (value & (value - 1)) != 0)
  {
    return -1;
  }

  unsigned long result = 0;
  FHECO_MSB_INDEX_UINT64(&result, value);
  return static_cast<int>(result);
}

inline int bit_size(std::uint64_t v)
{
  unsigned long msb_index;
  FHECO_MSB_INDEX_UINT64(&msb_index, v);
  return msb_index + 1;
}

void init_random(PackedVal &packed_val, integer slot_min, integer slot_max);

bool is_scalar(const PackedVal &packed_val);

void print_io_terms_values(const std::shared_ptr<ir::Func> &func, std::ostream &os);

void print_io_terms_values(
  const std::shared_ptr<ir::Func> &func, const ir::IOTermsInfo &inputs, const ir::IOTermsInfo &outputs,
  std::ostream &os);

void print_io_terms_values(const ir::IOTermsInfo &io_terms_values, std::size_t lead_trail_size, std::ostream &os);

void print_const_terms_values(
  const ir::ConstTermsValues &const_terms_values, std::size_t lead_trail_size, std::ostream &os);

void print_terms_values(const ir::TermsValues &terms_values, std::size_t lead_trail_size, std::ostream &os);

void print_packed_val(const PackedVal &packed_val, std::size_t lead_trail_size, std::ostream &os);

// from SEAL native/src/seal/util/common.h

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
inline constexpr T add_safe(T arg1, T arg2)
{
  if constexpr (std::is_unsigned<T>::value)
  {
    if (arg2 > (std::numeric_limits<T>::max)() - arg1)
      throw std::logic_error("unsigned overflow");
  }
  else
  {
    if (arg1 > 0 && (arg2 > (std::numeric_limits<T>::max)() - arg1))
      throw std::logic_error("signed overflow");
    else if (arg1 < 0 && (arg2 < (std::numeric_limits<T>::min)() - arg1))
      throw std::logic_error("signed underflow");
  }
  return static_cast<T>(arg1 + arg2);
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
inline constexpr T sub_safe(T arg1, T arg2)
{
  if constexpr (std::is_unsigned<T>::value)
  {
    if (arg1 < arg2)
      throw std::logic_error("unsigned underflow");
  }
  else
  {
    if (arg1 < 0 && (arg2 > (std::numeric_limits<T>::max)() + arg1))
      throw std::logic_error("signed underflow");
    else if (arg1 > 0 && (arg2 < (std::numeric_limits<T>::min)() + arg1))
      throw std::logic_error("signed overflow");
  }
  return static_cast<T>(arg1 - arg2);
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
inline constexpr T mul_safe(T arg1, T arg2)
{
  if constexpr (std::is_unsigned<T>::value)
  {
    if (arg1 && (arg2 > (std::numeric_limits<T>::max)() / arg1))
      throw std::logic_error("unsigned overflow");
  }
  else
  {
    // Positive inputs
    if ((arg1 > 0) && (arg2 > 0) && (arg2 > (std::numeric_limits<T>::max)() / arg1))
      throw std::logic_error("signed overflow");

    // #if (SEAL_COMPILER == SEAL_COMPILER_MSVC) && !defined(SEAL_USE_IF_CONSTEXPR)
    // #pragma warning(push)
    // #pragma warning(disable : 4146)
    // #endif

    // Negative inputs
    else if ((arg1 < 0) && (arg2 < 0) && ((-arg2) > (std::numeric_limits<T>::max)() / (-arg1)))
      throw std::logic_error("signed overflow");
    // Negative arg1; positive arg2
    else if ((arg1 < 0) && (arg2 > 0) && (arg2 > (std::numeric_limits<T>::max)() / (-arg1)))
      throw std::logic_error("signed underflow");

    // #if (SEAL_COMPILER == SEAL_COMPILER_MSVC) && !defined(SEAL_USE_IF_CONSTEXPR)
    // #pragma warning(pop)
    // #endif

    // Positive arg1; negative arg2
    else if ((arg1 > 0) && (arg2 < 0) && (arg2 < (std::numeric_limits<T>::min)() / arg1))
      throw std::logic_error("signed underflow");
  }
  return static_cast<T>(arg1 * arg2);
}

template <typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
inline std::size_t popcount(T x)
{
  static_assert(std::numeric_limits<T>::radix == 2, "non-binary type");

  constexpr int bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;
  std::bitset<bitwidth> bs(x);
  return bs.count();
}

// from https://stackoverflow.com/a/17050528
template <typename T>
std::vector<std::vector<T>> cart_product(const std::vector<std::vector<T>> &v)
{
  std::vector<std::vector<T>> s = {{}};
  for (const auto &u : v)
  {
    std::vector<std::vector<T>> r;
    for (const auto &x : s)
    {
      for (const auto y : u)
      {
        r.push_back(x);
        r.back().push_back(y);
      }
    }
    s = std::move(r);
  }
  return s;
}

std::ostream &operator<<(std::ostream &os, const ir::IOTermsInfo &io_terms_values);

std::ostream &operator<<(std::ostream &os, const ir::ConstTermsValues &const_terms_values);

std::ostream &operator<<(std::ostream &os, const ir::TermsValues &terms_values);

std::ostream &operator<<(std::ostream &os, const PackedVal &packed_val);
} // namespace fheco::util
