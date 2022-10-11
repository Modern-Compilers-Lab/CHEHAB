#pragma once

#include<memory>
#include<string>
#include<variant>
#include<vector>
#include"plaintext.hpp"
#include"ciphertext.hpp"
#include<optional>
#include"scalar.hpp"
#include"ir_const.hpp"

namespace ir
{

/*
using PlaintextRef = std::reference_wrapper<const datatype::Plaintext>;

using CiphertextRef = std::reference_wrapper<const datatype::Ciphertext>;

using ScalarRef = std::reference_wrapper<datatype::Scalar>;
*/

/*
using ConstantValue = std::variant<datatype::Scalar, datatype::Plaintext, datatype::Ciphertext>; //be careful order is important here

using TermValue = std::variant<ConstantValue, Operation>; //be careful order is important here
*/
class Term
{

  public:

  using Ptr = std::shared_ptr<Term>;

  class Operation
  {
    //we refer to FHE Operation the same as FHE Instruction
    private:

    OpCode op;
    
    std::vector<Ptr> operands;

    public:

    Operation() = delete;

    Operation(const Operation& ) = default;
    Operation& operator=(const Operation& ) = default;

    Operation(OpCode _op, const std::vector<Ptr>& _operands) : operands(_operands), op(_op) {}
    
    OpCode get_opcode() const
    { return this->op; }
    
    std::vector<Ptr>& get_operands() { return this->operands; }

    void replace_operation(const Operation& replace)
    {
      if(this == &replace) return; //here we are comparing addresses, so if it's the same object then there is no replace
      *this = replace;
    }

    void add_operand(Ptr operand) { this->operands.push_back(operand); }
  };


  private:

  int term_id;

  TermType type;
  
  std::optional<Operation> operation;
  
  std::string label;

  bool is_input=false;
  
  bool is_output=false;

  public:

  Term() = default;

  Term(const Term& term_copy) = default;
  Term& operator=(const Term& term_copy) = default;

  
  Term(const fhecompiler::Ciphertext& ct): label(ct.get_label()), type(ir::ciphertextType) {}

  Term(const fhecompiler::Plaintext& pt): label(pt.get_label()), type(ir::plaintextType) {}

  Term(const fhecompiler::Scalar& sc): label(sc.get_label()), type(ir::scalarType) {}

  Term(const Operation& operation_value, std::string label_value): operation(operation_value), label(label_value) {}
  
  bool merge_with_node(Ptr node_to_merge_with); 

  std::optional<Operation> get_operation_attribute() const { return this->operation; }

  void set_operation_attribute(const Operation& oprt) { this->operation = oprt; }

  void set_iutput_flag(bool input_flag) { this->is_input=input_flag; }

  void set_output_flag(bool output_flag) { this->is_output=output_flag; } 

  void set_term_type(TermType term_type) { this->type = term_type; }

  std::string get_label() const { return this->label; }

  bool get_input_flag() const { return this->is_input; }

  bool get_output_flag() const { return this->is_output; }

};



} // namespace ir
