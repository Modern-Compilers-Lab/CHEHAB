#pragma once

#include "datatypes_const.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include <iostream>
#include <string>
#include <variant>

namespace fhecompiler
{

class Scalar
{

public:
  using ScalarValueType = std::variant<int64_t, double, uint64_t>;

private:
  ScalarValueType data = 0;

  std::string label; // symbol

  static size_t scalar_id;

public:
  Scalar();
  Scalar(int64_t _data);
  Scalar(uint64_t _data);
  Scalar(double _data);
  Scalar(int _data) : Scalar((int64_t)_data) {}
  Scalar(const std::string &tag);

  Scalar(const Scalar &sc_copy);
  Scalar &operator=(const Scalar &sc_copy);

  Scalar(Scalar &&sc_move);
  Scalar &operator=(Scalar &&sc_move);

  Scalar &operator+=(const Scalar &rhs);

  Scalar &operator*=(const Scalar &rhs);

  Scalar &operator-=(const Scalar &rhs);

  Scalar operator-();

  friend Scalar operator+(const Scalar &lhs, const Scalar &rhs);

  friend Scalar operator-(const Scalar &lhs, const Scalar &rhs);

  friend Scalar operator*(const Scalar &lhs, const Scalar &rhs);

  friend Scalar operator-(const Scalar &rhs);

  void reduce(std::uint64_t plaintext_modulus);

  std::string get_label() const { return this->label; }

  void set_label(std::string label_value) { this->label = label_value; }

  void set_new_label();

  friend void compound_operate(Scalar &lhs, const Scalar &rhs, ir::OpCode);
};

} // namespace fhecompiler
