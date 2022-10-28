#pragma once

#include "datatypes_const.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace fhecompiler
{

class Ciphertext
{

private:
  std::string label; // symbol

  static size_t ciphertext_id;

public:
  Ciphertext() = delete;
  Ciphertext(const Plaintext &) = delete;
  Ciphertext(Plaintext) = delete;

  static Ciphertext encrypt(const Plaintext &pt);

  Ciphertext(const std::string &tag, VarType var_type = VarType::temp);

  Ciphertext(const Ciphertext &ct_copy);
  Ciphertext &operator=(const Ciphertext &ct_copy);

  Ciphertext(Ciphertext &&ct_move);
  Ciphertext &operator=(Ciphertext &&ct_move);

  Ciphertext &operator+=(const Ciphertext &rhs);

  Ciphertext &operator*=(const Ciphertext &rhs);

  Ciphertext &operator-=(const Ciphertext &rhs);

  Ciphertext &operator<<=(int steps);

  Ciphertext &operator>>=(int steps);

  Ciphertext &square(const Scalar &rhs);

  Ciphertext &exponentiate(uint64_t rhs);

  Ciphertext &rotate(int steps);

  Ciphertext operator-();

  friend Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs);

  friend Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs);

  friend Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs);

  friend Ciphertext operator<<(const Ciphertext &lhs, int steps);

  friend Ciphertext operator>>(const Ciphertext &lhs, int steps);

  friend Ciphertext operator-(const Ciphertext &rhs);

  friend Ciphertext exponentiate(const Ciphertext &lhs, uint64_t rhs);

  friend Ciphertext square(const Ciphertext lhs, const Scalar &rhs);

  friend Ciphertext rotate(const Ciphertext &rhs, uint64_t steps);

  void reduce(std::uint64_t plaintext_modulus);

  std::string get_label() const { return this->label; }

  void set_label(std::string label_value) { this->label = label_value; }

  std::string get_term_tag();

  void set_new_label();

  bool is_output() const;

  friend void compound_operate(Ciphertext &lhs, const Ciphertext &rhs, ir::OpCode);
};

} // namespace fhecompiler
