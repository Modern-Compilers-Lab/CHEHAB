#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace fheco
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

  Plaintext(std::string label, PackedVal example_val);

  Plaintext(std::string label, integer example_val_slot_min, integer example_val_slot_max);

  Plaintext(PackedVal packed_val);

  static ir::TermType term_type() { return ir::TermType::plaintext; }

  const Plaintext &set_output(std::string label) const;

  inline const size_t id() const { return id_; }

  inline const std::optional<PackedVal> &example_val() const { return example_val_; }

private:
  std::size_t id_;

  std::optional<PackedVal> example_val_;

  friend class ir::Function;
};
} // namespace fheco
