#pragma once

#include "ir_const.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>

namespace translator
{

/* return types map, it maps IR types with corresponding literal for code generation that targets the API */
std::unordered_map<ir::TermType, const char *> types_map = {

  {ir::ciphertextType, "Ciphertext"}, {ir::plaintextType, "Plaintext"}

};

/* ops_map maps IR operation code with corresponding literal for code generation that targets the API */
std::unordered_map<ir::OpCode, const char *> ops_map = {

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

std::unordered_map<ir::TermType, const char *> inputs_map_by_type = {
  {ir::TermType::scalarType, "ScalarInputs"},
  {ir::TermType::plaintextType, "EncodedInputs"},
  {ir::TermType::ciphertextType, "EncryptedInputs"}};

std::unordered_map<ir::TermType, const char *> inputs_map_identifier_by_type = {
  {ir::TermType::scalarType, "scalar_inputs"},
  {ir::TermType::plaintextType, "plain_inputs"},
  {ir::TermType::ciphertextType, "cipher_inputs"}};

std::unordered_map<ir::TermType, const char *> outputs_map_identifier_by_type = {
  {ir::TermType::scalarType, "scalar_outputs"},
  {ir::TermType::plaintextType, "plain_outputs"},
  {ir::TermType::ciphertextType, "cipher_outputs"}};

std::unordered_map<ir::TermType, const char *> outputs_map_by_type = {
  {ir::TermType::scalarType, "ScalarOutputs"},
  {ir::TermType::plaintextType, "EncodedOutputs"},
  {ir::TermType::ciphertextType, "EncryptedOutputs"}};

/* literals related to api/backend */
const char *scalar_int = "uint64_t";
const char *scalar_float = "double";
const char *encode_literal = "Encode";
const char *decode_literal = "Decode";
const char *encrypt_literal = "Encrypt";
const char *decrypt_literal = "Decrypt";
const char *context_type_literal = "Context";
const char *context_identifier = "context";
const char *public_key_literal = "PublicKey";
const char *secret_key_literal = "SecretKey";
const char *secret_key_identifier = "secret_key";
const char *relin_keys_type_literal = "RelinKeys";
const char *relin_keys_identifier = "relin_keys";
const char *galois_keys_type_literal = "GaloisKeys";
const char *galois_keys_identifier = "galois_keys";

/* general literals for C++ */
const char *new_line = "\n";
const char *end_of_command = ";";
const char *open_parantesis = "(";
const char *close_parantesis = ")";
const char *return_stmnt = "return ";
const char *assign_literal = "=";
const char *start_block = "{";
const char *end_block = "}";

// for containers
const char *unordered_map_literal = "std::unorderded_map";
const char *map_literal = "std::map";
const char *vector_literal = "std::vector";

enum class ContainerType
{
  vector,
  map,
  unorderedMap
};

std::vector<const char *> containers_map = {vector_literal, map_literal, unordered_map_literal};

struct CONTAINER_OF
{
  std::string operator()(ContainerType container_type, const char *type_literal)
  {
    std::string container_type_str(containers_map[static_cast<int>(container_type)]);
    std::string type_literal_str(type_literal);
    return container_type_str + "<" + type_literal_str + "> ";
  }
};

enum class AccessType
{
  readOnly,
  readAndModify
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

inline std::string stringfy_string(std::string &str)
{
  return '"' + str + '"';
}

} // namespace translator
