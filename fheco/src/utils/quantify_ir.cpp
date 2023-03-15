#include "quantify_ir.hpp"
#include <set>

using namespace std;

map<tuple<ir::OpCode, ir::TermType, ir::TermType>, size_t> utils::count_node_types(ir::Program *program)
{
  map<tuple<ir::OpCode, ir::TermType, ir::TermType>, std::size_t> result = {
    {{ir::OpCode::undefined, ir::TermType::undefined, ir::TermType::undefined}, 0}};

  if (program == nullptr)
    return result;

  const vector<ir::Program::Ptr> &nodes = program->get_dataflow_sorted_nodes(true);
  for (const ir::Program::Ptr &node : nodes)
  {
    if (node->is_operation_node() == false)
    {
      tuple<ir::OpCode, ir::TermType, ir::TermType> key = {
        node->get_opcode(), ir::TermType::undefined, ir::TermType::undefined};
      auto it = result.find(key);
      if (it == result.end())
        result.insert({key, 1});
      else
        it->second += 1;
    }
    else if (node->get_operands().size() == 1)
    {
      tuple<ir::OpCode, ir::TermType, ir::TermType> key = {
        node->get_opcode(), node->get_operands()[0]->get_term_type(), ir::TermType::undefined};
      auto it = result.find(key);
      if (it == result.end())
        result.insert({key, 1});
      else
        it->second += 1;
    }
    else if (node->get_operands().size() == 2)
    {
      tuple<ir::OpCode, ir::TermType, ir::TermType> key = {
        node->get_opcode(), node->get_operands()[0]->get_term_type(), node->get_operands()[1]->get_term_type()};
      auto it = result.find(key);
      if (it == result.end())
        result.insert({key, 1});
      else
        it->second += 1;
    }
  }
  return result;
}

set<tuple<ir::OpCode, ir::TermType, ir::TermType>> cartesian_product(
  set<ir::OpCode> opcodes, set<ir::TermType> arg1s, set<ir::TermType> arg2s)
{
  set<tuple<ir::OpCode, ir::TermType, ir::TermType>> result;
  for (const ir::OpCode &opcode : opcodes)
  {
    for (const ir::TermType &arg1 : arg1s)
    {
      for (const ir::TermType &arg2 : arg2s)
      {
        result.insert({opcode, arg1, arg2});
      }
    }
  }
  return result;
}

set<tuple<ir::OpCode, ir::TermType, ir::TermType>> all_cartesian_product()
{
  set<tuple<ir::OpCode, ir::TermType, ir::TermType>> result;
  for (int i = 0; i < static_cast<int>(ir::OpCode::decode); ++i)
  {
    for (int j = 0; j < static_cast<int>(ir::TermType::plaintext); ++j)
    {
      for (int k = 0; k < static_cast<int>(ir::TermType::plaintext); ++k)
      {
        result.insert({static_cast<ir::OpCode>(i), static_cast<ir::TermType>(j), static_cast<ir::TermType>(k)});
      }
    }
  }
  return result;
}

map<string, size_t> utils::count_main_node_classes(ir::Program *program)
{
  map<tuple<ir::OpCode, ir::TermType, ir::TermType>, size_t> node_types_count_result = count_node_types(program);

  map<string, set<tuple<ir::OpCode, ir::TermType, ir::TermType>>> class_type_mapping = {
    {"cipher_cipher_mul", {{ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::ciphertext}}},
    {"cipher_plain_mul",
     {{ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::plaintext},
      {ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::scalar},
      {ir::OpCode::mul, ir::TermType::plaintext, ir::TermType::ciphertext},
      {ir::OpCode::mul, ir::TermType::scalar, ir::TermType::ciphertext}}},
    {"he_square", {{ir::OpCode::square, ir::TermType::ciphertext, ir::TermType::undefined}}},
    {"he_rotate",
     {{ir::OpCode::rotate, ir::TermType::ciphertext, ir::TermType::rawData},
      {ir::OpCode::rotate, ir::TermType::rawData, ir::TermType::ciphertext}}},
    {"he_add_sub",
     {{ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::ciphertext},
      {ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::plaintext},
      {ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::scalar},
      {ir::OpCode::add, ir::TermType::plaintext, ir::TermType::ciphertext},
      {ir::OpCode::add, ir::TermType::scalar, ir::TermType::ciphertext},
      {ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::ciphertext},
      {ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::plaintext},
      {ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::scalar},
      {ir::OpCode::sub, ir::TermType::plaintext, ir::TermType::ciphertext},
      {ir::OpCode::sub, ir::TermType::scalar, ir::TermType::ciphertext},
      {ir::OpCode::negate, ir::TermType::ciphertext, ir::TermType::undefined}}},
    {"plain_plain_op",
     {cartesian_product(
       {ir::OpCode::mul, ir::OpCode::square, ir::OpCode::rotate, ir::OpCode::add, ir::OpCode::sub, ir::OpCode::negate},
       {ir::TermType::plaintext, ir::TermType::scalar}, {ir::TermType::plaintext, ir::TermType::scalar})}},
    {"dag_size", all_cartesian_product()}};
  map<string, size_t> result;

  for (const auto &c : class_type_mapping)
  {
    result[c.first] = 0;
    for (const auto &e : node_types_count_result)
    {
      auto it = c.second.find(e.first);
      if (it != c.second.end())
        result[c.first] += e.second;
    }
  }
  return result;
}
