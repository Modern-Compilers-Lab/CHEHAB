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
  
  private:

  int term_id;
  
  TermType type;
  
  OpCode opcode = undefined;

  std::optional<std::vector<Ptr>> operands;

  std::string label;

  bool is_input=false;
  
  bool is_output=false;

  public:

  Term() = delete;

  ~Term() {}

  Term(const Term& term_copy) = default;
  Term& operator=(const Term& term_copy) = default;
  
  Term(const fhecompiler::Ciphertext& ct): label(ct.get_label()), type(ir::ciphertextType) {}

  Term(const fhecompiler::Plaintext& pt): label(pt.get_label()), type(ir::plaintextType) {}

  Term(const fhecompiler::Scalar& sc): label(sc.get_label()), type(ir::scalarType) {}

  Term(OpCode _opcode, const std::vector<Ptr>& _operands, std::string label_value): opcode(_opcode), operands(std::make_optional<std::vector<Ptr>>(std::move(_operands))), label(label_value) {}
  
  bool merge_with_node(Ptr node_to_merge_with); 

  const std::optional<std::vector<Ptr>>& get_operands() const { return this->operands; }

  OpCode get_opcode() const { return this->opcode; }

  void add_operand(const Ptr& operand );

  void set_iutput_flag(bool input_flag) { this->is_input=input_flag; }

  void set_output_flag(bool output_flag) { this->is_output=output_flag; } 

  void set_term_type(TermType term_type) { this->type = term_type; }

  std::string get_label() const { return this->label;  }

  bool get_input_flag() const { return this->is_input; }

  bool get_output_flag() const { return this->is_output; }

};

} // namespace ir
