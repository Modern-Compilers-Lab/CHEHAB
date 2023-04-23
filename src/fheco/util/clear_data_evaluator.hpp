#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace fheco
{
namespace util
{
  // "modular SIMD arithmetic" like
  class ClearDataEvaluator
  {
  public:
    ClearDataEvaluator(std::size_t slot_count, integer modulus, bool signedness, bool delayed_reduction)
      : slot_count_{slot_count}, modulus_{modulus}, signedness_{signedness}, delayed_reduction_{delayed_reduction}
    {}

    void adjust_packed_val(PackedVal &packed_val) const;

    void adjust_scalar_val(ScalarVal &scalar_val) const;

    PackedVal make_random_packed_val(integer slot_min, integer slot_max) const;

    PackedVal make_packed_val(ScalarVal scalar_val) const;

    template <typename TArg, typename TDestination>
    void operate_unary(const ir::OpCode &op_code, const TArg &arg, TDestination &dest) const;

    template <typename TArg1, typename TArg2, typename TDestination>
    void operate_binary(const ir::OpCode &op_code, const TArg1 &arg1, const TArg2 &arg2, TDestination &dest) const;

    inline std::size_t slot_count() const { return slot_count_; }

    inline integer modulus() const { return modulus_; }

    inline bool signedness() const { return signedness_; }

    inline bool delayed_reduction() const { return delayed_reduction_; }

  private:
    void reduce(PackedVal &packed_val) const;

    void reduce(ScalarVal &scalar_val) const;

    void add(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;
    void add(const PackedVal &arg1, ScalarVal arg2, PackedVal &dest) const;
    void add(ScalarVal arg1, const PackedVal &arg2, PackedVal &dest) const;
    void add(ScalarVal arg1, ScalarVal arg2, ScalarVal &dest) const;

    void sub(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;
    void sub(const PackedVal &arg1, ScalarVal arg2, PackedVal &dest) const;
    void sub(ScalarVal arg1, const PackedVal &arg2, PackedVal &dest) const;
    void sub(ScalarVal arg1, ScalarVal arg2, ScalarVal &dest) const;

    void negate(const PackedVal &arg, PackedVal &dest) const;
    void negate(ScalarVal arg, ScalarVal &dest) const;

    void rotate(const PackedVal &arg, int steps, PackedVal &dest) const;
    inline void rotate(ScalarVal arg, int steps, ScalarVal &dest) const {}

    void mul(const PackedVal &arg1, const PackedVal &arg2, PackedVal &dest) const;
    void mul(const PackedVal &arg1, ScalarVal arg2, PackedVal &dest) const;
    void mul(ScalarVal arg1, const PackedVal &arg2, PackedVal &dest) const;
    void mul(ScalarVal arg1, ScalarVal arg2, ScalarVal &dest) const;

    std::size_t slot_count_;
    integer modulus_;
    bool signedness_;
    bool delayed_reduction_;
  };
} // namespace util
} // namespace fheco
