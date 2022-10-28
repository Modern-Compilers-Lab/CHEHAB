#pragma once

#include "ir_const.hpp"
#include <fstream>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>

#define INLINE inline

namespace translator
{
inline std::string stringfy_string(const std::string &str);
/* return types map, it maps IR types with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::TermType, const char *> types_map = {

  {ir::ciphertextType, "Ciphertext"}, {ir::plaintextType, "Plaintext"}

};

/* ops_map maps IR operation code with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::OpCode, const char *> ops_map = {

  {ir::OpCode::encrypt, "encrypt"},
  {ir::OpCode::add, "add"},
  {ir::OpCode::add_plain, "add_plain"},
  {ir::OpCode::assign, "="},
  {ir::OpCode::exponentiate, "exponentiate"},
  {ir::OpCode::negate, "negate"},
  {ir::OpCode::modswitch, "modswitch"},
  {ir::OpCode::mul, "multiply"},
  {ir::OpCode::mul_plain, "multiply_plain"},
  {ir::OpCode::sub, "sub"},
  {ir::OpCode::sub_plain, "sub_plain"},
  {ir::OpCode::rescale, "rescale"},
  {ir::OpCode::square, "square"},
  {ir::OpCode::rotate, "rotate"},
  {ir::OpCode::relinearize, "relinearize"}

};

/*
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
*/

/* literals related to api/backend */
INLINE const char *outputs_class_identifier = "outputs";
INLINE const char *outputs_class_literal = "Outputs";
INLINE const char *inputs_class_identifier = "inputs";
INLINE const char *inputs_class_literal = "Inputs";

INLINE const char *scalar_int = "uint64_t";
INLINE const char *scalar_float = "double";
INLINE const char *encode_literal = "encode";
INLINE const char *decode_literal = "decode";
INLINE const char *encrypt_literal = "encrypt";
INLINE const char *decrypt_literal = "decrypt";
INLINE const char *context_type_literal = "Context";
INLINE const char *context_identifier = "context";
INLINE const char *public_key_literal = "PublicKey";
INLINE const char *public_key_identifier = "public_key";
INLINE const char *secret_key_literal = "SecretKey";
INLINE const char *secret_key_identifier = "secret_key";
INLINE const char *relin_keys_type_literal = "RelinKeys";
INLINE const char *relin_keys_identifier = "relin_keys";
INLINE const char *galois_keys_type_literal = "GaloisKeys";
INLINE const char *galois_keys_identifier = "galois_keys";
INLINE const char *evaluator_type_literal = "Evaluator";
INLINE const char *evaluator_identifier = "evaluator";
INLINE const char *encoder_type_literal = "BatchEncoder";
INLINE const char *encoder_type_identifier = "encoder";
INLINE const char *encryptor_type_literal = "Encryptor";
INLINE const char *encryptor_type_identifier = "encryptor";
INLINE const char *insert_object_instruction = "insert"; // instruction to insert inputs/outputs

INLINE std::unordered_map<ir::TermType, const char *> get_instruction_by_type = {
  {ir::plaintextType, "get_plaintext"}, {ir::ciphertextType, "get_ciphertext"}};

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

template <typename T>
inline void dump_vector(const std::vector<T> &v, std::string &vector_str)
{
  vector_str = '{';
  for (size_t i = 0; i < v.size(); i++)
  {
    vector_str += std::to_string(v[i]);
    if (i < v.size() - 1)
      vector_str += ',';
  }
  vector_str += '}';
}

struct Encoder
{
private:
  std::string context_id;

public:
  bool is_defined = false;

  Encoder(const std::string &ctxt_id) : context_id(ctxt_id) {}

  template <typename T>
  void write_encode(
    const std::vector<T> &clear_message, const std::string &identifier, std::ostream &os,
    const char *constant_type) const
  {
    std::string vector_str;
    dump_vector(clear_message, vector_str);
    CONTAINER_OF container_of;
    std::string vector_identifier = identifier + "_clear";
    os << container_of(ContainerType::vector, constant_type) << " " << vector_identifier << " = " << vector_str << ";"
       << '\n';
    os << types_map[ir::plaintextType] << " " << identifier << ";" << '\n';
    os << encoder_type_identifier << "." << encode_literal << "(" << vector_identifier << "," << identifier << ");"
       << '\n';
  }

  friend std::ostream &operator<<(std::ostream &os, const Encoder &encoder);
};

struct Encryptor
{
private:
  std::string context_id;
  std::string public_key_id;
  std::optional<std::string> private_key_id;

public:
  bool is_defined = false;
  Encryptor(const std::string &ctxt_id, const std::string &pbk_id, const std::optional<std::string> &pvk_id)
    : context_id(ctxt_id), public_key_id(pbk_id), private_key_id(pvk_id)
  {}

  void write_encrypt(const std::string &cipher_dest_id, const std::string &plaintext_id, std::ostream &os)
  {
    os << types_map[ir::ciphertextType] << " " << cipher_dest_id << end_of_command << '\n';
    os << encryptor_type_identifier << "." << encrypt_literal << "(" << plaintext_id << "," << cipher_dest_id << ")"
       << end_of_command << '\n';
  }

  friend std::ostream &operator<<(std::ostream &os, const Encryptor &encryptor);
};

INLINE std::ostream &operator<<(std::ostream &os, const Encryptor &encryptor)
{
  if (encryptor.private_key_id != std::nullopt)
  {
    os << encryptor_type_literal << " " << encryptor_type_identifier << "(" << encryptor.context_id << ","
       << encryptor.public_key_id << "," << *(encryptor.private_key_id) << ")";
  }
  else
  {
    os << encryptor_type_literal << " " << encryptor_type_identifier << "(" << encryptor.context_id << ","
       << encryptor.public_key_id << ")";
  }
  return os;
}

INLINE std::ostream &operator<<(std::ostream &os, const Evaluator &evaluator)
{
  os << evaluator_type_literal << " " << evaluator.evaluator_id << "(" << evaluator.context_id << ")";
  return os;
}

INLINE std::ostream &operator<<(std::ostream &os, const Encoder &encoder)
{
  os << encoder_type_literal << " " << encoder_type_identifier << "(" << encoder.context_id << ")";
  return os;
}

inline void write_input(const std::string &input_identifier, ir::TermType type, std::ostream &os)
{
  // retrieve an input from object
  os << types_map[type] << " " << input_identifier << " = " << inputs_class_identifier << "."
     << get_instruction_by_type[type] << "(" << stringfy_string(input_identifier) << ")" << end_of_command << '\n';
}

inline void write_output(const std::string &output_identifier, ir::TermType type, std::ostream &os)
{
  // insert output in object
  os << outputs_class_identifier << "." << insert_object_instruction << "<" << types_map[type] << ">"
     << "({" << stringfy_string(output_identifier) << "," << output_identifier << "})" << end_of_command << '\n';
}

inline std::string stringfy_string(const std::string &str)
{
  return '"' + str + '"';
}

} // namespace translator