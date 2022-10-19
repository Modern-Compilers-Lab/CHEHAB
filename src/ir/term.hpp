#pragma once

#include "ciphertext.hpp"
#include "ir_const.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <memory>
#include <optional>
#include <string>
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

  std::optional<std::vector<Ptr>> operands;

  std::string label;

public:
  Term() = delete;

  ~Term() {}

  Term(const Term &term_copy) = default;
  Term &operator=(const Term &term_copy) = default;

  Term(const fhecompiler::Ciphertext &ct) : label(ct.get_label()), type(ir::ciphertextType) {}

  Term(const fhecompiler::Plaintext &pt) : label(pt.get_label()), type(ir::plaintextType) {}

  Term(const fhecompiler::Scalar &sc) : label(sc.get_label()), type(ir::scalarType) {}

  Term(OpCode _opcode, const std::vector<Ptr> &_operands, std::string label_value)
    : opcode(_opcode), operands(std::make_optional<std::vector<Ptr>>(std::move(_operands))), label(label_value)
  {}

  bool merge_with_node(Ptr node_to_merge_with);

  const std::optional<std::vector<Ptr>> &get_operands() const { return this->operands; }

  OpCode get_opcode() const { return this->opcode; }

  void add_operand(const Ptr &operand);

  void set_term_type(TermType term_type) { this->type = term_type; }

  std::string get_label() const { return this->label; }
};

} // namespace ir
