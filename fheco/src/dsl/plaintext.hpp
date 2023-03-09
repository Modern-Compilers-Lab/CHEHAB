#pragma once

#include "datatypes_const.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace fhecompiler
{

class Plaintext
{
private:
  std::string label; // symbol

  static std::size_t plaintext_id;

public:
  Plaintext();

  Plaintext(const std::vector<std::int64_t> &message);

  Plaintext(const std::vector<std::uint64_t> &message);

  Plaintext(const std::vector<double> &message);

  Plaintext(const std::string &tag, VarType var_type = VarType::temp);

  Plaintext(const Plaintext &pt_copy);
  Plaintext &operator=(const Plaintext &pt_copy);

  Plaintext &operator=(Plaintext &&pt_move);
  Plaintext(Plaintext &&pt_move) = default;

  Plaintext &operator+=(const Plaintext &rhs);

  Plaintext &operator*=(const Plaintext &rhs);

  Plaintext &operator-=(const Plaintext &rhs);

  // Plaintext operator-();

  friend Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs);

  friend Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs);

  friend Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs);

  // friend Plaintext operator-(const Plaintext &rhs);

  std::string get_label() const { return this->label; }

  void set_label(std::string label_value) { this->label = label_value; }

  void set_new_label();
};

} // namespace fhecompiler
