#pragma once

#include"datatypes_const.hpp"
#include"ir_const.hpp"
#include<variant>
#include<iostream>


namespace fhecompiler
{

class Scalar
{
  
  public:

  using ScalarValueType = std::variant<int64_t, double>;

  private:

  ScalarValueType data=0;
 
  std::string label; //symbol

  static size_t scalar_id;

  public:

  Scalar();
  Scalar(int64_t _data);
  Scalar(double _data);
  Scalar(int _data): Scalar((int64_t) _data) {}
  
  Scalar(std::string tag, bool output_flag=false, bool input_flag=true);

  Scalar(const Scalar& sc_copy);
  Scalar& operator=(const Scalar& sc_copy);
  

  Scalar& operator+=(const Scalar& rhs);

  Scalar& operator*=(const Scalar& rhs);

  Scalar& operator-=(const Scalar& rhs);

  friend Scalar operator+(Scalar& lhs, const Scalar& rhs);

  friend Scalar operator-(Scalar& lhs, const Scalar& rhs);

  friend Scalar operator*(Scalar& lhs, const Scalar& rhs);

  friend Scalar operator-(Scalar& rhs);
  
  void reduce(std::uint64_t plaintext_modulus);
  
  std::string get_label() const { return this->label; }

  void set_label(std::string label_value )  { this->label = label_value; }
  
  void set_as_output() const;

  friend inline void set_new_label(Scalar& sc);

  friend void compound_operate(Scalar& lhs, const Scalar& rhs, ir::OpCode);

};

} // namespace fhecompiler
