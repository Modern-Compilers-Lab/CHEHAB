#pragma once

#include "ir_const.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace fhecompiler
{
class Scalar
{
public:
  Scalar() : label_(term_type().auto_name_prefix() + std::to_string(id_++)) {}

  Scalar(std::int64_t data, std::string tag);

  Scalar(std::int64_t data);

  Scalar(int data, std::string tag) : Scalar(static_cast<std::int64_t>(data), std::move(tag)) {}

  Scalar(int data) : Scalar(static_cast<std::int64_t>(data)) {}

  Scalar(uint64_t data, std::string tag);

  Scalar(uint64_t data);

  Scalar(const Scalar &) = default;

  Scalar &operator=(const Scalar &) = default;

  Scalar(Scalar &&) = default;

  Scalar &operator=(Scalar &&) = default;

  static inline ir::TermType term_type() { return ir::TermType::scalar; }

  const Scalar &tag(std::string tag) const;

  const std::string &label() const { return label_; }

  inline const ir::ScalarValue &example_value() const { return example_value_; }

private:
  inline ir::ScalarValue &example_value() { return example_value_; }

  static std::size_t id_;

  std::string label_;

  ir::ScalarValue example_value_;
};
} // namespace fhecompiler
