#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace fheco
{
namespace ir
{
  class Function;
} // namespace ir

class Ciphertext
{
public:
  // terms ids start from 1
  Ciphertext() : id_{0}, dim_{0}, idx_{}, example_val_{} {}

  explicit Ciphertext(std::string label);

  Ciphertext(std::string label, PackedVal example_val);

  Ciphertext(std::string label, integer example_val_slot_min, integer example_val_slot_max);

  ~Ciphertext() = default;

  Ciphertext(const Ciphertext &other) = default;

  Ciphertext(Ciphertext &&other) = default;

  Ciphertext &operator=(const Ciphertext &other);

  Ciphertext &operator=(Ciphertext &&other);

  const Ciphertext operator[](std::size_t idx) const;

  Ciphertext &operator[](std::size_t idx);

  static ir::TermType term_type() { return ir::TermType::ciphertext; }

  const Ciphertext &set_output(std::string label) const;

  inline std::size_t id() const { return id_; }

  inline int dim() const { return dim_; }

  inline const std::vector<std::size_t> &idx() const { return idx_; }

  inline const std::optional<PackedVal> &example_val() const { return example_val_; }

private:
  std::size_t id_;

  int dim_;

  std::vector<std::size_t> idx_;

  std::optional<PackedVal> example_val_;

  friend class ir::Function;
  friend Ciphertext emulate_subscripted_read(const Ciphertext &arg);
  friend void emulate_subscripted_write(Ciphertext &lhs, const Ciphertext &rhs);
};
} // namespace fheco
