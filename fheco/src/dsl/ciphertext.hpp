#pragma once

#include "ir_const.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fhecompiler
{
class Ciphertext
{
private:
  std::string label_; // symbol

  std::optional<std::string> tag_;

  ir::VectorValue example_value_;

  static size_t ciphertext_id;

public:
  Ciphertext() {}

  Ciphertext(const std::string &tag, long long min_value = 0, long long ming_value = 100);

  Ciphertext(const std::string &tag, const ir::VectorValue &example_value);

  Ciphertext(const Ciphertext &) = default;

  Ciphertext &operator=(const Ciphertext &) = default;

  Ciphertext(Ciphertext &&) = default;

  Ciphertext &operator=(Ciphertext &&) = default;

  Ciphertext &set_output(const std::string &tag);

  const std::string &get_label() const { return label_; }

  const std::optional<std::string> &get_tag() const { return tag_; }

  void set_label(const std::string &label) { label_ = label; }

  inline ir::VectorValue &example_value() { return example_value_; }

  inline const ir::VectorValue &example_value() const { return example_value_; }

  void set_new_label();

  bool is_output() const;
};
} // namespace fhecompiler
