#pragma once

#include "common.hpp"
#include "term_type.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  class Function;
} // namespace ir

class Plaintext
{
public:
  Plaintext() {}

  explicit Plaintext(std::string label);

  Plaintext(std::string label, const ir::VectorValue &example_value);

  Plaintext(std::string label, std::int64_t example_value_slot_min, std::int64_t example_value_slot_max);

  Plaintext(const std::vector<std::int64_t> &value);

  Plaintext(const std::vector<std::uint64_t> &value);

  static ir::TermType term_type() { return ir::TermType::plaintext; }

  const Plaintext &set_output(std::string label) const;

  inline const size_t id() const { return id_; }

  inline const std::optional<ir::VectorValue> &example_value() const { return example_value_; }

private:
  inline void set_example_value(ir::VectorValue example_value) { example_value_ = std::move(example_value); }

  std::size_t id_;

  std::optional<ir::VectorValue> example_value_;

  friend class ir::Function;
};
} // namespace fhecompiler
