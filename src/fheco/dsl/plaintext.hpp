#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/ir/common.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace fheco
{
class Ciphertext;

namespace ir
{
  class Function;
} // namespace ir

class Plaintext
{
public:
  // terms ids start from 1
  Plaintext() : id_{0}, dim_{0}, idx_{}, example_val_{} {}

  explicit Plaintext(std::string label);

  Plaintext(std::string label, PackedVal example_val);

  Plaintext(std::string label, integer example_val_slot_min, integer example_val_slot_max);

  Plaintext(PackedVal packed_val);

  Plaintext(integer scalar_val);

  ~Plaintext() = default;

  Plaintext(const Plaintext &other) = default;

  Plaintext(Plaintext &&other) = default;

  Plaintext &operator=(const Plaintext &other);

  Plaintext &operator=(Plaintext &&other);

  Plaintext operator[](std::size_t idx) const;

  Plaintext &operator[](std::size_t idx);

  static ir::TermType term_type() { return ir::TermType::plaintext; }

  const Plaintext &set_output(std::string label) const;

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
  friend Plaintext emulate_subscripted_read(const Plaintext &arg);
  friend void emulate_subscripted_write(Plaintext &lhs, const Plaintext &rhs);
};
} // namespace fheco
