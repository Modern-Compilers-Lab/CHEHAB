#pragma once

#include<string>
#include<variant>
#include<vector>
#include"ir_const.hpp"
#include"datatypes_const.hpp"
#include"fhecompiler_const.hpp"
#include"plaintext.hpp"
#include<iostream>

namespace fhecompiler
{

class Ciphertext
{

  private:
 
  std::string label; //symbol

  static size_t ciphertext_id;

  public:

  Ciphertext() = delete;
  explicit Ciphertext(Plaintext& );

  Ciphertext(const Plaintext& ) = delete;

  Ciphertext(const std::string& tag, VarType var_type=VarType::temp);

  Ciphertext(const Ciphertext& ct_copy);
  Ciphertext& operator=(const Ciphertext& ct_copy);

  Ciphertext(Ciphertext&& ct_move) = default;
  Ciphertext& operator=(Ciphertext&& ct_move) = default;
  
  Ciphertext& operator+=(const Ciphertext& rhs);

  Ciphertext& operator*=(const Ciphertext& rhs);

  Ciphertext& operator-=(const Ciphertext& rhs);

  Ciphertext operator+(const Ciphertext& rhs);

  Ciphertext operator-(const Ciphertext& rhs);

  Ciphertext operator*(const Ciphertext& rhs);

  Ciphertext operator+(const Plaintext& rhs);

  Ciphertext operator-();
  
  friend Ciphertext operator+(const Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator-(const Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator*(const Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator-(const Ciphertext& rhs);
  
  void reduce(std::uint64_t plaintext_modulus);
  
  std::string get_label() const { return this->label; }

  void set_label(std::string label_value )  { this->label = label_value; }
  
  std::string get_term_tag();

  void set_new_label();

  bool is_output() const;

  friend void compound_operate(Ciphertext& lhs, const Ciphertext& rhs, ir::OpCode);
};

}// namespace fhecompiler