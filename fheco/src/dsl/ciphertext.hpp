#pragma once

#include "ir_const.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <string>

namespace fhecompiler
{
class Ciphertext
{
public:
  Ciphertext() : label_{term_type().auto_name_prefix() + std::to_string(id_++)} {}

  explicit Ciphertext(std::string tag);

  Ciphertext(std::string tag, long long slot_min_value, long long slot_max_value);

  Ciphertext(std::string tag, const ir::VectorValue &example_value);

  Ciphertext(const Ciphertext &) = default;

  Ciphertext &operator=(const Ciphertext &) = default;

  Ciphertext(Ciphertext &&) = default;

  Ciphertext &operator=(Ciphertext &&) = default;

  static ir::TermType term_type() { return ir::TermType::ciphertext; }

  const Ciphertext &tag(std::string tag) const;

  const Ciphertext &set_output(std::string tag) const;

  const std::string &label() const { return label_; }

  inline const ir::VectorValue &example_value() const { return example_value_; }

private:
  inline ir::VectorValue &example_value() { return example_value_; }

  static std::size_t id_;

  std::string label_;

  ir::VectorValue example_value_;
};
} // namespace fhecompiler
