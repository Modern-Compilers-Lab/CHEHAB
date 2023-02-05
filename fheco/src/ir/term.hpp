#pragma once

#include "ciphertext.hpp"
#include "ir_const.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <ostream>
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
  TermType type;

  static size_t term_id;

  OpCode opcode = OpCode::undefined;

  struct OperationAttribute
  {

    bool is_inplace = false; // true if the term represents an inplace instruction

    std::vector<Ptr> operands;

    OperationAttribute() = default;
    ~OperationAttribute() {}

    OperationAttribute(const std::vector<Ptr> &_operands) : operands(_operands) {}
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

  Term(const fhecompiler::Ciphertext &ct) : label(ct.get_label()), type(ir::ciphertextType) { term_id++; }

  Term(const fhecompiler::Plaintext &pt) : label(pt.get_label()), type(ir::plaintextType) { term_id++; }

  Term(const fhecompiler::Scalar &sc) : label(sc.get_label()), type(ir::scalarType) { term_id++; }

  Term(OpCode _opcode, const std::vector<Ptr> &_operands, const std::string &label_value)
    : operation_attribute({_operands}), label(label_value), opcode(_opcode)
  {

    for (auto &operand : _operands)
      operand->add_parent_label(this->label);

    term_id++;
  }

  // this constructure is useful in case of rawData where we store it in the lable as an int
  Term(const std::string &symbol, TermType term_type) : label(symbol), type(term_type) { term_id++; }

  Term(TermType term_type) : type(term_type) {}

  void reverse_operands()
  {
    if (!is_operation_node())
      return;
    reverse(operation_attribute->operands.begin(), operation_attribute->operands.end());
  }

  void clear_operands();

  void clear_parents() { parents_labels.clear(); }

  void add_parent_label(const std::string &label);

  void set_operands(const std::vector<Ptr> &_operands)
  {
    if (operation_attribute == std::nullopt)
      return;
    for (auto &operand : _operands)
    {
      operand->insert_parent_label(this->label);
    }
    (*operation_attribute).operands = _operands;
  }

  bool is_inplace() const { return (*operation_attribute).is_inplace; }

  void insert_parent_label(const std::string &label);

  void set_opcode(ir::OpCode _opcode) { opcode = _opcode; }

  void set_label(const std::string &_label) { label = _label; }

  void set_inplace() { (*operation_attribute).is_inplace = true; }

  void replace_with(const Ptr &rhs);

  bool is_operation_node() const { return operation_attribute != std::nullopt; }

  const std::vector<Ptr> &get_operands() const { return operation_attribute->operands; }

  const std::unordered_multiset<std::string> &get_parents_labels() { return this->parents_labels; }

  void set_parents_label(std::unordered_multiset<std::string> p_labels) { parents_labels = p_labels; }

  void delete_operand_term(const std::string &term_label);

  void delete_parent(const std::string &parent_label);

  void delete_operand_at_index(size_t index);

  void set_operand_at_index(size_t index, const Ptr &operand_ptr);

  OpCode get_opcode() const { return opcode; }

  void add_operand(const Ptr &operand);

  void set_term_type(TermType term_type) { this->type = term_type; }

  void replace_operand_with(const Ptr &operand, const Ptr &to_replace_with);

  size_t get_term_id() { return term_id; }

  std::string get_label() const { return this->label; }
  TermType get_term_type() const { return this->type; }

  void sort_operands(std::function<bool(const Ptr &, const Ptr &)> comp);

  void set_a_default_label();
};

} // namespace ir
