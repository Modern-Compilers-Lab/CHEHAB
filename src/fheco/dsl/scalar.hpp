#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/term_type.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>

namespace fheco
{
namespace ir
{
  class Function;
} // namespace ir

class Scalar
{
public:
  Scalar() {}

  Scalar(std::int64_t value);

  // Scalar(std::uint64_t value);

  static inline ir::TermType term_type() { return ir::TermType::scalar; }

  inline const size_t id() const { return id_; }

  inline const std::optional<ir::ScalarValue> &example_value() const { return example_value_; }

private:
  inline void set_example_value(ir::ScalarValue example_value) { example_value_ = std::move(example_value); }

  std::size_t id_;

  std::optional<ir::ScalarValue> example_value_;

  friend class ir::Function;
};
} // namespace fheco
