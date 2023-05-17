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
#include <vector>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
void init_random(PackedVal &packed_val, integer slot_min, integer slot_max);

void print_io_terms_values(const std::shared_ptr<ir::Func> &func, std::ostream &os);

void print_io_terms_values(
  const std::shared_ptr<ir::Func> &func, const ir::IOTermsInfo &inputs, const ir::IOTermsInfo &outputs,
  std::ostream &os);

void print_io_terms_values(const ir::IOTermsInfo &io_terms_values, std::size_t lead_trail_size, std::ostream &os);

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
} // namespace fheco::util

namespace std
{
ostream &operator<<(ostream &os, const fheco::ir::IOTermsInfo &io_terms_values);

ostream &operator<<(ostream &os, const fheco::ir::TermsValues &terms_values);

ostream &operator<<(ostream &os, const fheco::PackedVal &packed_val);
} // namespace std
