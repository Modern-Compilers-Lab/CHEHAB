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
  Ciphertext();
  Ciphertext(const Plaintext &) = delete;
  Ciphertext(Plaintext) = delete;

  static Ciphertext encrypt(const Plaintext &pt);

  Ciphertext(const std::string &tag, VarType var_type = VarType::temp);

  Ciphertext(const Ciphertext &ct_copy);
  Ciphertext &operator=(const Ciphertext &ct_copy);

  explicit Ciphertext(Ciphertext &&ct_move);
  Ciphertext &operator=(Ciphertext &&ct_move);

  Ciphertext &operator+=(const Ciphertext &rhs);

  Ciphertext &operator*=(const Ciphertext &rhs);

  Ciphertext &operator-=(const Ciphertext &rhs);

  Ciphertext &operator<<=(int steps);

  Ciphertext &operator>>=(int steps);

  Ciphertext &square();

  Ciphertext &exponentiate(uint64_t rhs);

  Ciphertext &rotate(int steps);

  Ciphertext &rotate_rows(int steps);

  Ciphertext &rotate_columns();

  Ciphertext operator-();

  friend Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs);
  friend Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs);
  friend Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs);
  friend Ciphertext operator<<(const Ciphertext &lhs, int steps);
  friend Ciphertext operator>>(const Ciphertext &lhs, int steps);
  friend Ciphertext operator-(const Ciphertext &rhs);
  friend Ciphertext exponentiate(const Ciphertext &lhs, uint32_t rhs);
  friend Ciphertext square(const Ciphertext &lhs);
  friend Ciphertext rotate(const Ciphertext &rhs, int steps);
  friend Ciphertext rotate_rows(const Ciphertext &lhs, int steps);
  friend Ciphertext rotate_columns(const Ciphertext &lhs);

  std::string get_label() const { return this->label; }

  void set_label(std::string label_value) { this->label = label_value; }

  std::string get_term_tag();

  void set_new_label();

  bool is_output() const;
};

Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs);
Ciphertext operator<<(const Ciphertext &lhs, int steps);
Ciphertext operator>>(const Ciphertext &lhs, int steps);
Ciphertext operator-(const Ciphertext &rhs);
Ciphertext exponentiate(const Ciphertext &lhs, uint32_t rhs);
Ciphertext square(const Ciphertext &lhs);
Ciphertext rotate(const Ciphertext &rhs, int steps);
Ciphertext rotate_rows(const Ciphertext &lhs, int steps);
Ciphertext rotate_columns(const Ciphertext &lhs);

} // namespace fhecompiler
