#include "fheco/util/quantify_ir.hpp"
#include <unordered_set>

using namespace std;

namespace fheco
{
namespace util
{
  size_t HashOpType::operator()(const OpType &op_type) const
  {
    size_t h = hash<ir::OpCode::Type>{}(op_type.op_code_type_);
    for (const auto &operand_type : op_type.operands_types_)
      ir::hash_combine(h, operand_type);
    return h;
  }

  bool EqualOpType::operator()(const OpType &lhs, const OpType &rhs) const
  {
    return lhs.op_code_type_ == rhs.op_code_type_ && lhs.operands_types_ == rhs.operands_types_;
  }

  unordered_map<OpType, size_t, HashOpType, EqualOpType> count_op_types(ir::Function &func)
  {
    unordered_map<OpType, size_t, HashOpType, EqualOpType> result{};
    for (const auto &term : func.get_top_sorted_terms())
    {
      vector<ir::TermType> operands_types{};
      for (const auto &operand : term->operands())
        operands_types.push_back(operand->type());
      auto [it, inserted] = result.emplace(OpType{term->op_code().type(), operands_types}, 1);
      if (!inserted)
        ++it->second;
    }
    return result;
  }

  map<string, size_t> group_main_op_types_count(
    const unordered_map<OpType, size_t, HashOpType, EqualOpType> &op_types_count)
  {
    unordered_map<string, unordered_set<OpType, HashOpType, EqualOpType>> main_op_types = {
      {"01_ctxt_ctxt_mul", {{ir::OpCode::Type::mul, {ir::TermType::ciphertext, ir::TermType::ciphertext}}}},
      {"02_he_square", {{ir::OpCode::Type::square, {ir::TermType::ciphertext}}}},
      {"03_encrypt",
       {{ir::OpCode::Type::encrypt, {ir::TermType::plaintext}}, {ir::OpCode::Type::encrypt, {ir::TermType::scalar}}}},
      {"04_relin", {{ir::OpCode::Type::relin, {ir::TermType::ciphertext}}}},
      {"05_he_rotate", {{ir::OpCode::Type::rotate, {ir::TermType::ciphertext}}}},
      {"06_ctxt_ptxt_mul",
       {{ir::OpCode::Type::mul, {ir::TermType::ciphertext, ir::TermType::plaintext}},
        {ir::OpCode::Type::mul, {ir::TermType::ciphertext, ir::TermType::scalar}},
        {ir::OpCode::Type::mul, {ir::TermType::plaintext, ir::TermType::ciphertext}},
        {ir::OpCode::Type::mul, {ir::TermType::scalar, ir::TermType::ciphertext}}}},
      {"07_mod_switch", {{ir::OpCode::Type::mod_switch, {ir::TermType::ciphertext}}}},
      {"08_he_add_sub_negate",
       {{ir::OpCode::Type::add, {ir::TermType::ciphertext, ir::TermType::ciphertext}},
        {ir::OpCode::Type::add, {ir::TermType::ciphertext, ir::TermType::plaintext}},
        {ir::OpCode::Type::add, {ir::TermType::ciphertext, ir::TermType::scalar}},
        {ir::OpCode::Type::add, {ir::TermType::plaintext, ir::TermType::ciphertext}},
        {ir::OpCode::Type::add, {ir::TermType::scalar, ir::TermType::ciphertext}},
        {ir::OpCode::Type::sub, {ir::TermType::ciphertext, ir::TermType::ciphertext}},
        {ir::OpCode::Type::sub, {ir::TermType::ciphertext, ir::TermType::plaintext}},
        {ir::OpCode::Type::sub, {ir::TermType::ciphertext, ir::TermType::scalar}},
        {ir::OpCode::Type::sub, {ir::TermType::plaintext, ir::TermType::ciphertext}},
        {ir::OpCode::Type::sub, {ir::TermType::scalar, ir::TermType::ciphertext}},
        {ir::OpCode::Type::negate, {ir::TermType::ciphertext}}}},
      {"09_ptxt_ptxt_ops",
       {{ir::OpCode::Type::mul, {ir::TermType::plaintext, ir::TermType::plaintext}},
        {ir::OpCode::Type::mul, {ir::TermType::plaintext, ir::TermType::scalar}},
        {ir::OpCode::Type::mul, {ir::TermType::scalar, ir::TermType::plaintext}},
        {ir::OpCode::Type::mul, {ir::TermType::scalar, ir::TermType::scalar}},
        {ir::OpCode::Type::square, {ir::TermType::plaintext}},
        {ir::OpCode::Type::square, {ir::TermType::scalar}},
        {ir::OpCode::Type::rotate, {ir::TermType::plaintext}},
        {ir::OpCode::Type::add, {ir::TermType::plaintext, ir::TermType::plaintext}},
        {ir::OpCode::Type::add, {ir::TermType::plaintext, ir::TermType::scalar}},
        {ir::OpCode::Type::add, {ir::TermType::scalar, ir::TermType::plaintext}},
        {ir::OpCode::Type::add, {ir::TermType::scalar, ir::TermType::scalar}},
        {ir::OpCode::Type::sub, {ir::TermType::plaintext, ir::TermType::plaintext}},
        {ir::OpCode::Type::sub, {ir::TermType::plaintext, ir::TermType::scalar}},
        {ir::OpCode::Type::sub, {ir::TermType::scalar, ir::TermType::plaintext}},
        {ir::OpCode::Type::sub, {ir::TermType::scalar, ir::TermType::scalar}},
        {ir::OpCode::Type::negate, {ir::TermType::plaintext}},
        {ir::OpCode::Type::negate, {ir::TermType::scalar}}}},
      {"10_leaves", {{ir::OpCode::Type::nop, {}}}},

    };

    map<string, size_t> result;
    for (const auto &main_type : main_op_types)
      result.emplace(main_type.first, 0);

    size_t captured_terms = 0;
    size_t all_terms = 0;
    for (const auto &e : op_types_count)
    {
      all_terms += e.second;
      for (const auto &main_type : main_op_types)
      {
        if (main_type.second.find(e.first) != main_type.second.end())
        {
          result[main_type.first] += e.second;
          captured_terms += e.second;
          // assuming main classes are disjoint
          break;
        }
      }
    }
    result.emplace("00_captured_terms", captured_terms);
    result.emplace("00_other", all_terms - captured_terms);
    result.emplace("00_all_terms", all_terms);
    return result;
  }
} // namespace util
} // namespace fheco

namespace std
{
ostream &operator<<(ostream &os, const map<string, size_t> &main_op_types_count)
{
  for (const auto &e : main_op_types_count)
    os << e.first << ": " << e.second << '\n';
  return os;
}

ostream &operator<<(
  ostream &os,
  const unordered_map<fheco::util::OpType, size_t, fheco::util::HashOpType, fheco::util::EqualOpType> &op_types_count)
{
  for (const auto &e : op_types_count)
  {
    os << "(" << fheco::ir::OpCode::sample_op_code_from_type(e.first.op_code_type_);
    for (auto operand_type : e.first.operands_types_)
      os << ", " << fheco::ir::term_type_str_repr(operand_type);
    os << "): " << e.second << '\n';
  }
  return os;
}
} // namespace std
