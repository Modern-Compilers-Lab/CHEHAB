#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/function.hpp"
#include <cstddef>
#include <limits>
#include <ostream>
#include <random>
#include <stdexcept>
#include <vector>

namespace fheco
{
namespace util
{
  void init_random(PackedVal &packed_val, integer slot_min, integer slot_max);

  void print_io_terms_values(const ir::Function &func, std::ostream &os);

  void print_io_terms_values(
    const ir::Function &func, const ir::IOTermsInfo &inputs, const ir::IOTermsInfo &outputs, std::ostream &os);

  void print_io_terms_values(const ir::IOTermsInfo &io_terms_values, std::size_t lead_trail_size, std::ostream &os);

  void print_packed_val(const PackedVal &packed_val, std::size_t lead_trail_size, std::ostream &os);

  // from SEAL native/src/seal/util/common.h

  inline constexpr integer add_safe(integer arg1, integer arg2)
  {
    if (arg1 > 0 && (arg2 > (std::numeric_limits<integer>::max)() - arg1))
      throw std::logic_error("signed overflow");
    else if (arg1 < 0 && (arg2 < (std::numeric_limits<integer>::min)() - arg1))
      throw std::logic_error("signed underflow");
    return static_cast<integer>(arg1 + arg2);
  }

  inline constexpr integer sub_safe(integer arg1, integer arg2)
  {

    if (arg1 < 0 && (arg2 > (std::numeric_limits<integer>::max)() + arg1))
      throw std::logic_error("signed underflow");
    else if (arg1 > 0 && (arg2 < (std::numeric_limits<integer>::min)() + arg1))
      throw std::logic_error("signed overflow");
    return static_cast<integer>(arg1 - arg2);
  }

  inline constexpr integer mul_safe(integer arg1, integer arg2)
  {
    // Positive inputs
    if ((arg1 > 0) && (arg2 > 0) && (arg2 > (std::numeric_limits<integer>::max)() / arg1))
      throw std::logic_error("signed overflow");
    // Negative inputs
    else if ((arg1 < 0) && (arg2 < 0) && ((-arg2) > (std::numeric_limits<integer>::max)() / (-arg1)))
      throw std::logic_error("signed overflow");
    // Negative arg1; positive arg2
    else if ((arg1 < 0) && (arg2 > 0) && (arg2 > (std::numeric_limits<integer>::max)() / (-arg1)))
      throw std::logic_error("signed underflow");
    // Positive arg1; negative arg2
    else if ((arg1 > 0) && (arg2 < 0) && (arg2 < (std::numeric_limits<integer>::min)() / arg1))
      throw std::logic_error("signed underflow");
    return static_cast<integer>(arg1 * arg2);
  }
} // namespace util
} // namespace fheco

namespace std
{
std::ostream &operator<<(std::ostream &os, const fheco::ir::IOTermsInfo &io_terms_values);

ostream &operator<<(ostream &os, const fheco::PackedVal &packed_val);
} // namespace std
