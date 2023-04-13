#pragma once

#include "ir_const.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace fhecompiler
{
class Plaintext
{
public:
  Plaintext() : label_{term_type().auto_name_prefix() + std::to_string(id_++)} {}

  explicit Plaintext(std::string tag);

  Plaintext(std::string tag, long long slot_min_value, long long slot_max_value);

  Plaintext(std::string tag, const ir::VectorValue &example_value);

  Plaintext(const std::vector<std::int64_t> &data, std::string tag);

  Plaintext(const std::vector<std::int64_t> &data);

  Plaintext(const std::vector<int> &data, std::string tag);

  Plaintext(const std::vector<int> &data);

  Plaintext(const std::vector<std::uint64_t> &data, std::string tag);

  Plaintext(const std::vector<std::uint64_t> &data);

  Plaintext(const Plaintext &) = default;

  Plaintext &operator=(const Plaintext &) = default;

  Plaintext &operator=(Plaintext &&) = default;

  Plaintext(Plaintext &&pt_move) = default;

  static ir::TermType term_type() { return ir::TermType::plaintext; }

  const Plaintext &tag(std::string tag) const;

  const Plaintext &set_output(std::string tag) const;

  const std::string &label() const { return label_; }

  inline const ir::VectorValue &example_value() const { return example_value_; }

private:
  inline ir::VectorValue &example_value() { return example_value_; }

  static std::size_t id_;

  std::string label_;

  ir::VectorValue example_value_;
};
} // namespace fhecompiler
