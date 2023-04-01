#pragma once

#include "ir_const.hpp"
#include <cstdint>
#include <string>

namespace fhecompiler
{
class Scalar
{
private:
  std::string label_; // symbol

  ir::ScalarValue example_value_;

  static size_t scalar_id;

public:
  Scalar() {}

  Scalar(int64_t data);

  Scalar(int data) : Scalar((int64_t)data) {}

  Scalar(uint64_t data);

  Scalar(const Scalar &) = default;

  Scalar &operator=(const Scalar &) = default;

  Scalar(Scalar &&) = default;

  Scalar &operator=(Scalar &&) = default;

  const std::string &get_label() const { return label_; }

  inline ir::ScalarValue &example_value() { return example_value_; }

  inline const ir::ScalarValue &example_value() const { return example_value_; }

  void set_label(const std::string &label) { label_ = label; }

  void set_new_label();
};
} // namespace fhecompiler
