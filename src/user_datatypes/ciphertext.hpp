#pragma once

#include<string>
#include<variant>
#include<vector>
#include"ir_const.hpp"
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

  Ciphertext(std::string tag, bool output_flag=false, bool input_flag=true);

  Ciphertext(const Ciphertext& ct_copy);
  Ciphertext& operator=(const Ciphertext& ct_copy);
  
  Ciphertext& operator+=(const Ciphertext& rhs);

  Ciphertext& operator*=(const Ciphertext& rhs);

  Ciphertext& operator-=(const Ciphertext& rhs);

  friend Ciphertext operator+(Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator-(Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator*(Ciphertext& lhs, const Ciphertext& rhs);

  friend Ciphertext operator-(Ciphertext& rhs);
  
  void reduce(std::uint64_t plaintext_modulus);
  
  std::string get_label() const { return this->label; }

  void set_label(std::string label_value )  { this->label = label_value; }
  
  void set_as_output() const;

  friend inline void set_new_label(Ciphertext& sc);

  friend void compound_operate(Ciphertext& lhs, const Ciphertext& rhs, ir::OpCode);
};

}// namespace fhecompiler