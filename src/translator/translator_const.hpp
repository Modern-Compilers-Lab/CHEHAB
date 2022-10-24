#pragma once

#include "ir_const.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>

#define INLINE inline

namespace translator
{

/* return types map, it maps IR types with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::TermType, const char *> types_map = {

  {ir::ciphertextType, "Ciphertext"}, {ir::plaintextType, "Plaintext"}

};

/* ops_map maps IR operation code with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::OpCode, const char *> ops_map = {

  {ir::OpCode::add, "add"},
  {ir::OpCode::assign, "="},
  {ir::OpCode::exponentiate, "exponentiate"},
  {ir::OpCode::negate, "negate"},
  {ir::OpCode::modswitch, "modswitch"},
  {ir::OpCode::mul, "mul"},
  {ir::OpCode::sub, "sub"},
  {ir::OpCode::rescale, "rescale"},
  {ir::OpCode::square, "square"},
  {ir::OpCode::rotate, "rotate"},
  {ir::OpCode::relinearize, "relinearize"}

};

INLINE std::unordered_map<ir::TermType, const char *> inputs_map_by_type = {
  {ir::TermType::scalarType, "ScalarInputs"},
  {ir::TermType::plaintextType, "EncodedInputs"},
  {ir::TermType::ciphertextType, "EncryptedInputs"}};

INLINE std::unordered_map<ir::TermType, const char *> inputs_map_identifier_by_type = {
  {ir::TermType::scalarType, "scalar_inputs"},
  {ir::TermType::plaintextType, "plain_inputs"},
  {ir::TermType::ciphertextType, "cipher_inputs"}};

INLINE std::unordered_map<ir::TermType, const char *> outputs_map_identifier_by_type = {
  {ir::TermType::scalarType, "scalar_outputs"},
  {ir::TermType::plaintextType, "plain_outputs"},
  {ir::TermType::ciphertextType, "cipher_outputs"}};

INLINE std::unordered_map<ir::TermType, const char *> outputs_map_by_type = {
  {ir::TermType::scalarType, "ScalarOutputs"},
  {ir::TermType::plaintextType, "EncodedOutputs"},
  {ir::TermType::ciphertextType, "EncryptedOutputs"}};

/* literals related to api/backend */
INLINE const char *scalar_int = "uint64_t";
INLINE const char *scalar_float = "double";
INLINE const char *encode_literal = "Encode";
INLINE const char *decode_literal = "Decode";
INLINE const char *encrypt_literal = "Encrypt";
INLINE const char *decrypt_literal = "Decrypt";
INLINE const char *context_type_literal = "Context";
INLINE const char *context_identifier = "context";
INLINE const char *public_key_literal = "PublicKey";
INLINE const char *secret_key_literal = "SecretKey";
INLINE const char *secret_key_identifier = "secret_key";
INLINE const char *relin_keys_type_literal = "RelinKeys";
INLINE const char *relin_keys_identifier = "relin_keys";
INLINE const char *galois_keys_type_literal = "GaloisKeys";
INLINE const char *galois_keys_identifier = "galois_keys";
INLINE const char *evaluator_type_literal = "Evaluator";
INLINE const char *evaluator_identifier = "evaluator";

/* general literals for C++ */
INLINE const char *new_line = "\n";
INLINE const char *end_of_command = ";";
INLINE const char *open_parantesis = "(";
INLINE const char *close_parantesis = ")";
INLINE const char *return_stmnt = "return ";
INLINE const char *assign_literal = "=";
INLINE const char *start_block = "{";
INLINE const char *end_block = "}";

// for containers
INLINE const char *unordered_map_literal = "std::unorderded_map";
INLINE const char *map_literal = "std::map";
INLINE const char *vector_literal = "std::vector";

enum class ContainerType
{
  vector,
  map,
  unorderedMap
};

INLINE std::vector<const char *> containers_map = {vector_literal, map_literal, unordered_map_literal};

enum class AccessType
{
  readOnly,
  readAndModify
};

struct CONTAINER_OF
{
  std::string operator()(ContainerType container_type, const char *type_literal)
  {
    std::string container_type_str(containers_map[static_cast<int>(container_type)]);
    std::string type_literal_str(type_literal);
    return container_type_str + "<" + type_literal_str + "> ";
  }
};

struct ARGUMENT
{
  const std::string arg_identifier;
  const std::string arg_type;
  AccessType access_type;

  ~ARGUMENT() {}

  ARGUMENT(const char *type, const char *identifier, AccessType _access_type)
    : arg_type(type), arg_identifier(identifier), access_type(_access_type)
  {}
};

struct ARGUMENTS_LIST
{
public:
  std::string operator()(const std::vector<ARGUMENT> &args)
  {
    std::string arguments_list_str("(");
    size_t n_arguments = args.size();
    for (size_t i = 0; i < n_arguments; i++)
    {
      std::string ref_tag = "&";
      std::string const_tag = (args[i].access_type == AccessType::readOnly ? "const " : "");
      if (i == n_arguments - 1)
      {
        arguments_list_str += (const_tag + args[i].arg_type + ref_tag + " " + args[i].arg_identifier);
      }
      else
        arguments_list_str += (const_tag + args[i].arg_type + ref_tag + " " + args[i].arg_identifier + ", ");
    }
    arguments_list_str += ")";
    return arguments_list_str;
  }
};

struct MAP_ACCESSOR
{
  std::string operator()(const std::string &map_identifier, const std::string &key)
  {
    return map_identifier + "[" + key + "]";
  }
};

struct Evaluator
{

private:
  std::string context_id;
  std::string evaluator_id;

public:
  Evaluator(const std::string &ctxt_id, const std::string eval_id) : context_id(ctxt_id), evaluator_id(eval_id) {}

  void write_binary_operation(
    ir::TermType type, ir::OpCode opcode, std::string &destination, const std::string &lhs, const std::string &rhs,
    std::ofstream &os) const
  {
    std::string object_type = types_map[type];
    std::string operator_identifier = ops_map[opcode];
    os << object_type << " " << destination << ";" << '\n';
    os << evaluator_id << "." << operator_identifier << "(" << lhs << "," << rhs << "," << destination << ");" << '\n';
  }

  friend std::ostream &operator<<(std::ostream &o_stream, const Evaluator &evaluator);
};

INLINE std::ostream &operator<<(std::ostream &o_stream, const Evaluator &evaluator)
{
  o_stream << evaluator_type_literal << " " << evaluator.evaluator_id << "(" << evaluator.context_id << ")";
  return o_stream;
}

inline std::string stringfy_string(std::string &str)
{
  return '"' + str + '"';
}

} // namespace translator
