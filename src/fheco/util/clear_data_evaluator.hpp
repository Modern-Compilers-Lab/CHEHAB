#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace fheco::util
{
// "modular SIMD arithmetic" like
class ClearDataEvaluator
{
public:
  ClearDataEvaluator(std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction)
    : slot_count_{slot_count}, modulus_{modulus}, signedness_{signedness}, delayed_reduction_{delayed_reduction}
  {}

  void adjust_packed_val(PackedVal &packed_val) const;

  PackedVal make_rand_packed_val(integer slot_min, integer slot_max) const;

  void operate(const ir::OpCode &op_code, const std::vector<PackedVal> &args, PackedVal &dest) const;

  void operate_unary(const ir::OpCode &op_code, const PackedVal &arg, PackedVal &dest) const;

  void operate_binary(const ir::OpCode &op_code, const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;

  inline std::size_t slot_count() const { return slot_count_; }

  inline integer modulus() const { return modulus_; }

  inline bool signedness() const { return signedness_; }

  inline bool delayed_reduction() const { return delayed_reduction_; }

private:
  void reduce(PackedVal &packed_val) const;

  void add(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;

  void sub(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;

  void negate(const PackedVal &arg, PackedVal &dest) const;

  void rotate(const PackedVal &arg, int steps, PackedVal &dest) const;

  void mul(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;

  std::size_t slot_count_;
  integer modulus_;
  bool signedness_;
  bool delayed_reduction_;
};
} // namespace fheco::util
