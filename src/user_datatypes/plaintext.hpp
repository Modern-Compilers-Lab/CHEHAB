#pragma once

#include<string>
#include<variant>
#include<vector>
#include"ir_const.hpp"
#include<iostream>

namespace fhecompiler
{

class Plaintext
{
  
  public:

  using MessageType = std::variant<int64_t, double>;

  private:
 
  std::string label; //symbol

  static size_t plaintext_id;

  public:
  Plaintext() = delete;
  Plaintext(const std::vector<int64_t>& message);
  Plaintext(const std::vector<double>& message);
  Plaintext(const std::vector<int>& message): Plaintext((const std::vector<int64_t>& ) message) {}
  
  Plaintext(std::string tag, bool output_flag=false, bool input_flag=false);

  Plaintext(const Plaintext& pt_copy);
  Plaintext& operator=(const Plaintext& pt_copy);
  

  Plaintext& operator+=(const Plaintext& rhs);

  Plaintext& operator*=(const Plaintext& rhs);

  Plaintext& operator-=(const Plaintext& rhs);

  Plaintext operator+(const Plaintext& rhs);

  Plaintext operator*(const Plaintext& rhs);

  Plaintext operator-(const Plaintext& rhs);

  Plaintext operator-();

  friend Plaintext operator+(const Plaintext& lhs, const Plaintext& rhs);

  friend Plaintext operator-(const Plaintext& lhs, const Plaintext& rhs);

  friend Plaintext operator*(const Plaintext& lhs, const Plaintext& rhs);

  friend Plaintext operator-(const Plaintext& rhs);
  
  void reduce(std::uint64_t plaintext_modulus);
  
  std::string get_label() const { return this->label; }

  void set_label(std::string label_value )  { this->label = label_value; }
  
  void set_as_output(const std::string& label = "") const;

  friend inline void set_new_label(Plaintext& pt);
  
  static std::string generate_new_label();

  friend void compound_operate(Plaintext& lhs, const Plaintext& rhs, ir::OpCode);
};

}// namespace fhecompiler