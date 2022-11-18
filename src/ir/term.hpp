#pragma once

#include "ciphertext.hpp"
#include "ir_const.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <algorithm>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace ir
{

class Term
{

public:
  using Ptr = std::shared_ptr<Term>;

private:
  int term_id;

  TermType type;

  struct OperationAttribute
  {

    OpCode opcode;

    bool is_inplace = false; // true if the term represents an inplace instruction

    std::vector<Ptr> operands;

    OperationAttribute() = default;
    ~OperationAttribute() {}

    OperationAttribute(OpCode _opcode, const std::vector<Ptr> &_operands) : opcode(_opcode), operands(_operands) {}
  };

  std::optional<OperationAttribute> operation_attribute;

  std::unordered_multiset<std::string> parents_labels;

  std::string label;

public:
  Term() = delete;

  ~Term() {}

  Term(const Term &term_copy) = default;
  Term &operator=(const Term &term_copy) = default;

  Term(Term &&term_move) = default;
  Term &operator=(Term &&term_move) = default;

  Term(const fhecompiler::Ciphertext &ct) : label(ct.get_label()), type(ir::ciphertextType) {}

  Term(const fhecompiler::Plaintext &pt) : label(pt.get_label()), type(ir::plaintextType) {}

  Term(const fhecompiler::Scalar &sc) : label(sc.get_label()), type(ir::scalarType) {}

  Term(OpCode _opcode, const std::vector<Ptr> &_operands, const std::string &label_value)
    : operation_attribute({_opcode, _operands}), label(label_value)
  {}

  // this constructure is useful in case of rawData where we store it in the lable as an int
  Term(const std::string &symbol, TermType term_type) : label(symbol), type(term_type) {}

  void reverse_operands()
  {
    if (!is_operation_node())
      return;
    reverse(operation_attribute->operands.begin(), operation_attribute->operands.end());
  }

  void clear_operands()
  {
    if (operation_attribute == std::nullopt)
      return;
    (*operation_attribute).operands.clear();
  }

  void set_operands(const std::vector<Ptr> &_operands)
  {
    if (operation_attribute == std::nullopt)
      return;

    (*operation_attribute).operands = _operands;
  }

  bool is_inplace() const { return (*operation_attribute).is_inplace; }

  void insert_parent_label(const std::string &label);

  bool merge_with_node(Ptr node_to_merge_with);

  void set_opcode(ir::OpCode _opcode) { (*operation_attribute).opcode = _opcode; }

  void set_label(const std::string &_label) { label = _label; }

  void set_inplace() { (*operation_attribute).is_inplace = true; }

  void replace_with(const ir::Term &rhs) { *this = rhs; /* calling copy assignement operator*/ }

  void replace_with(const Ptr &rhs) { *this = *(rhs.get()); /* calling copy assignement operator*/ }

  bool is_operation_node() const { return operation_attribute != std::nullopt; }

  const std::vector<Ptr> &get_operands() const { return operation_attribute->operands; }

  const std::unordered_multiset<std::string> &get_parents_labels() { return this->parents_labels; }

  void delete_operand_term(const std::string &term_label);

  void delete_parent(const std::string &parent_label);

  std::shared_ptr<Ptr> make_copy_ptr();

  OpCode get_opcode() const { return (*operation_attribute).opcode; }

  void add_operand(const Ptr &operand);

  void set_term_type(TermType term_type) { this->type = term_type; }

  std::string get_label() const { return this->label; }
  TermType get_term_type() const { return this->type; }
};

} // namespace ir
