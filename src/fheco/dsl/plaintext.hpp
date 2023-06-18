#pragma once

#include "fheco/dsl/common.hpp"
#include "fheco/dsl/compiler.hpp"
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
  explicit Plaintext(std::vector<std::size_t> shape = {Compiler::active_func()->slot_count()});

  explicit Plaintext(std::string label, std::vector<std::size_t> shape = {Compiler::active_func()->slot_count()});

  Plaintext(
    std::string label, PackedVal example_val, std::vector<std::size_t> shape = {Compiler::active_func()->slot_count()});

  Plaintext(
    std::string label, integer example_val_slot_min, integer example_val_slot_max,
    std::vector<std::size_t> shape = {Compiler::active_func()->slot_count()});

  Plaintext(PackedVal packed_val, std::vector<std::size_t> shape = {Compiler::active_func()->slot_count()});

  Plaintext(integer scalar_val);

  ~Plaintext() = default;

  Plaintext(const Plaintext &other) = default;

  Plaintext(Plaintext &&other) = default;

  Plaintext &operator=(const Plaintext &other);

  Plaintext &operator=(Plaintext &&other);

  const Plaintext operator[](std::size_t idx) const;

  Plaintext &operator[](std::size_t idx);

  const Plaintext &set_output(std::string label) const;

  inline void set_scalar() { set_shape({}); }

  void set_shape(std::vector<std::size_t> shape);

  inline std::size_t id() const { return id_; }

  inline const std::vector<std::size_t> &shape() const { return shape_; }

  inline const std::vector<std::size_t> &idx() const { return idx_; }

  inline const std::optional<PackedVal> &example_val() const { return example_val_; }

private:
  // terms ids start from 1
  std::size_t id_;

  std::vector<std::size_t> shape_;

  std::vector<std::size_t> idx_;

  std::optional<PackedVal> example_val_;

  friend class ir::Func;
  friend Ciphertext emulate_subscripted_read(const Ciphertext &arg);
  friend void emulate_subscripted_write(Ciphertext &lhs, const Ciphertext &rhs);
  friend Plaintext emulate_subscripted_read(const Plaintext &arg);
  friend void emulate_subscripted_write(Plaintext &lhs, const Plaintext &rhs);
};
} // namespace fheco
