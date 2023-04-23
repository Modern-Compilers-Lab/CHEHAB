#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <cstddef>
#include <optional>
#include <stdint.h>
#include <string>

namespace fheco
{
namespace ir
{
  class Function;
} // namespace ir

class Ciphertext
{
public:
  Ciphertext() {}

  explicit Ciphertext(std::string label);

  Ciphertext(std::string label, PackedVal example_val);

  Ciphertext(std::string label, integer example_val_slot_min, integer example_val_slot_max);

  static ir::TermType term_type() { return ir::TermType::ciphertext; }

  const Ciphertext &set_output(std::string label) const;

  inline const size_t id() const { return id_; }

  inline const std::optional<PackedVal> &example_val() const { return example_val_; }

private:
  std::size_t id_;

  std::optional<PackedVal> example_val_;

  friend class ir::Function;
};
} // namespace fheco
