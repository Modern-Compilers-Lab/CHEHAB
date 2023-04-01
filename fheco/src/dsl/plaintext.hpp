#pragma once

#include "ir_const.hpp"
#include <optional>
#include <string>
#include <vector>

namespace fhecompiler
{

class Plaintext
{
private:
  std::string label_; // symbol

  std::optional<std::string> tag_;

  ir::VectorValue example_value_;

  static std::size_t plaintext_id;

public:
  Plaintext() {}

  Plaintext(const std::string &tag, long long min_value = 0, long long max_value = 100);

  Plaintext(const std::string &tag, const ir::VectorValue &example_value);

  Plaintext(const std::vector<std::int64_t> &data);

  Plaintext(const std::vector<std::uint64_t> &data);

  Plaintext(const Plaintext &) = default;

  Plaintext &operator=(const Plaintext &) = default;

  Plaintext &operator=(Plaintext &&) = default;

  Plaintext(Plaintext &&pt_move) = default;

  Plaintext &set_output(const std::string &tag);

  const std::string &get_label() const { return label_; }

  const std::optional<std::string> &get_tag() const { return tag_; }

  inline ir::VectorValue &example_value() { return example_value_; }

  inline const ir::VectorValue &example_value() const { return example_value_; }

  void set_label(const std::string &label) { label_ = label; }

  void set_new_label();
};

} // namespace fhecompiler
