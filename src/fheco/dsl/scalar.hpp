#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>

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

  Scalar(integer value);

  static inline ir::TermType term_type() { return ir::TermType::scalar; }

  inline const size_t id() const { return id_; }

  inline const std::optional<ScalarVal> &example_val() const { return example_val_; }

private:
  std::size_t id_;

  std::optional<ScalarVal> example_val_;

  friend class ir::Function;
};
} // namespace fheco
