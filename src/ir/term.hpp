#pragma once

#include "ciphertext.hpp"
#include "ir_const.hpp"
#include "maped_list.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
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

  OpCode opcode;

  std::optional<utils::MapedDoublyLinkedList<std::string, Ptr>> operands;

  std::unordered_set<std::string> parents_labels;

  std::string label;

public:
  Term() = delete;

  ~Term()
  {
    if (operands != std::nullopt)
      (*operands).clear(); /* Term is the owner of operands list */
  }

  Term(const Term &term_copy) = default;
  Term &operator=(const Term &term_copy) = default;

  Term(Term &&term_move) = default;
  Term &operator=(Term &&term_move) = default;

  Term(const fhecompiler::Ciphertext &ct) : label(ct.get_label()), type(ir::ciphertextType) {}

  Term(const fhecompiler::Plaintext &pt) : label(pt.get_label()), type(ir::plaintextType) {}

  Term(const fhecompiler::Scalar &sc) : label(sc.get_label()), type(ir::scalarType) {}

  Term(OpCode _opcode, const utils::MapedDoublyLinkedList<std::string, Ptr> &_operands, const std::string &label_value)
    : opcode(_opcode),
      operands(std::make_optional<utils::MapedDoublyLinkedList<std::string, Ptr>>(std::move(_operands))),
      label(label_value)
  {}

  // this constructure is useful in case of rawData where we store it in the lable as an int
  Term(const std::string &symbol, TermType term_type) : label(symbol), type(term_type) {}

  void insert_parent_label(const std::string &label);

  bool merge_with_node(Ptr node_to_merge_with);

  const std::optional<utils::MapedDoublyLinkedList<std::string, Ptr>> &get_operands() const { return this->operands; }

  const std::unordered_set<std::string> &get_parents_labels() { return this->parents_labels; }

  void delete_operand_term(const std::string &term_label);

  std::shared_ptr<Ptr> make_copy_ptr();

  OpCode get_opcode() const { return this->opcode; }

  void add_operand(const Ptr &operand);

  void set_term_type(TermType term_type) { this->type = term_type; }

  std::string get_label() const { return this->label; }
  TermType get_term_type() const { return this->type; }
};

} // namespace ir
