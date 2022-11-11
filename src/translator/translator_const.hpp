#pragma once

#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
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
  {ir::OpCode::rotate, "rotate_vector"},
  {ir::OpCode::rotate_rows, "rotate_rows"},
  {ir::OpCode::rotate_columns, "rotate_columns"},
  {ir::OpCode::relinearize, "relinearize"}

};

/* literals related to api/backend */
INLINE const char *params_type_literal = "EncryptionParameters";
INLINE const char *params_identifier_literal = "params";
INLINE const char *scalar_int = "int64_t";
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
INLINE const char *context_function_name = "create_context";
INLINE const char *set_plain_modulus_intruction = "set_plain_modulus";
INLINE const char *set_coef_modulus_instruction = "set_coeff_modulus";
INLINE const char *set_poly_modulus_degree_instruction = "set_poly_modulus_degree";
INLINE const char *encrypted_inputs_class_literal = "std::unordered_map<std::string, Ciphertext>";
INLINE const char *encoded_inputs_class_literal = "std::unordered_map<std::string, Plaintext>";
INLINE const char *encoded_outputs_class_literal = "std::unordered_map<std::string, Plaintext>";
INLINE const char *encrypted_outputs_class_literal = "std::unordered_map<std::string, Ciphertext>";

INLINE std::unordered_map<ir::TermType, const char *> outputs_class_identifier = {
  {ir::plaintextType, "encoded_outputs"}, {ir::ciphertextType, "encrypted_outputs"}};

INLINE std::unordered_map<ir::TermType, const char *> inputs_class_identifier = {
  {ir::plaintextType, "encoded_inputs"}, {ir::ciphertextType, "encrypted_inputs"}};

INLINE std::unordered_map<ir::OpCode, const char *> get_other_args_by_opcode = {
  {ir::OpCode::rotate, galois_keys_identifier},
  {ir::OpCode::exponentiate, relin_keys_identifier},
  {ir::OpCode::rotate_rows, galois_keys_identifier},
  {ir::OpCode::rotate_columns, galois_keys_identifier},
  {ir::OpCode::relinearize, relin_keys_identifier}};

enum class AccessType
{
  readOnly,
  readAndModify
};

struct Argument
{
  const std::string arg_identifier;
  const std::string arg_type;
  AccessType access_type;

  Argument() = default;

  ~Argument() {}

  Argument(const char *type, const char *identifier, AccessType _access_type)
    : arg_type(type), arg_identifier(identifier), access_type(_access_type)
  {}
};

struct ArgumentList
{
public:
  ArgumentList() = default;

  ~ArgumentList() {}

  std::string operator()(const std::vector<Argument> &args)
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

struct EncryptionWriter
{
private:
  std::string public_key_identifier;
  std::string context_identifier;
  std::string encryptor_identifier;
  std::string encryptor_type_literal;
  std::string encrypt_instruction_literal;
  bool is_init = false;

public:
  EncryptionWriter() = default;

  EncryptionWriter(
    const std::string &encryptor_type, const std::string &encryptor_id, const std::string &encrypt_instruction,
    const std::string &pk_id, const std::string &ctxt_id)
    : public_key_identifier(pk_id), context_identifier(ctxt_id), encryptor_identifier(encryptor_id),
      encrypt_instruction_literal(encrypt_instruction), encryptor_type_literal(encryptor_type)
  {}
  ~EncryptionWriter() {}

  void init(std::ostream &os)
  {
    is_init = true;
    os << encryptor_type_literal << " " << encryptor_identifier << "(" << context_identifier << ","
       << public_key_identifier << ");" << '\n';
  }

  void write_encryption(std::ostream &os, const std::string &plaintext_id, const std::string &destination_cipher) const
  {
    os << types_map[ir::ciphertextType] << " " << destination_cipher << ";" << '\n';
    os << encryptor_identifier << "." << encrypt_instruction_literal << "(" << plaintext_id << "," << destination_cipher
       << ");" << '\n';
  }

  bool is_initialized() const { return this->is_init; }
};

struct EvaluationWriter
{
private:
  std::string evaluator_identifier;
  std::string context_identifier;
  std::string evaluator_type_literal;
  bool is_init = false;

public:
  EvaluationWriter() = default;

  EvaluationWriter(const std::string &evaluator_type, const std::string &evaluator_id, const std::string &context_id)
    : evaluator_identifier(evaluator_id), context_identifier(context_id), evaluator_type_literal(evaluator_type)
  {}

  ~EvaluationWriter() {}

  void init(std::ostream &os)
  {
    is_init = true;
    os << evaluator_type_literal << " " << evaluator_identifier << "(" << context_identifier << ");" << '\n';
  }

  void write_unary_operation(
    std::ostream &os, ir::OpCode opcode, const std::string &destination_id, const std::string &lhs_id,
    ir::TermType type)
  {
    if (is_init == false)
      init(os);

    std::string other_args("");
    auto it = get_other_args_by_opcode.find(opcode);
    if (it != get_other_args_by_opcode.end())
    {
      other_args = it->second;
    }

    os << types_map[type] << " " << destination_id << ";";
    if (other_args.length() == 0)
    {
      os << evaluator_identifier << "." << ops_map[opcode] << "(" << lhs_id << "," << destination_id << ");";
    }
    else
    {
      os << evaluator_identifier << "." << ops_map[opcode] << "(" << lhs_id << "," << other_args << ","
         << destination_id << ");";
    }
  }

  void write_binary_operation(
    std::ostream &os, ir::OpCode opcode, const std::string &destination_id, const std::string &lhs_id,
    const std::string &rhs_id, ir::TermType type)
  {

    if (is_init == false)
      init(os);

    os << types_map[type] << " " << destination_id << ";" << '\n';
    std::string other_args("");
    auto it = get_other_args_by_opcode.find(opcode);
    if (it != get_other_args_by_opcode.end())
    {
      other_args = it->second;
    }
    os << evaluator_identifier << "." << ops_map[opcode] << "(" << lhs_id << "," << rhs_id;
    if (other_args.length() > 0)
    {
      os << ", " << other_args;
    }
    os << "," << destination_id << ");" << '\n';
  }

  bool is_initialized() const { return this->is_init; }
};

struct EncodingWriter
{
private:
  std::string encoder_type_literal;
  std::string encoder_identifier;
  std::string context_identifier;
  std::string encoder_instruction_literal;
  bool is_init = false;

public:
  EncodingWriter() = default;

  EncodingWriter(
    const std::string &encoder_type, const std::string &encoder_id, const std::string &ctxt_id,
    const std::string &encode_inst_literal)
    : encoder_type_literal(encoder_type), encoder_identifier(encoder_id), context_identifier(ctxt_id),
      encoder_instruction_literal(encode_inst_literal)
  {}
  ~EncodingWriter() {}

  void init(std::ostream &os)
  {
    is_init = true;
    os << encoder_type_literal << " " << encoder_identifier << "(" << context_identifier << ");" << '\n';
  }

  void write_scalar_encoding(
    std::ostream &os, const std::string &plaintext_id, const std::string &scalar_value, const std::string &scalar_type,
    const std::string &vector_size) const
  {
    // create a vector
    const std::string vector_id = plaintext_id + "_clear";
    os << "vector<" << scalar_type << ">"
       << " " << vector_id << "(" << vector_size << ");" << '\n';

    os << "for(size_t i = 0; i < " << vector_size << "; i++)" << '\n'
       << "{" << '\n'
       << vector_id << "[i] = " << scalar_value << ";" << '\n'
       << "}" << '\n';

    os << types_map[ir::plaintextType] << " " << plaintext_id << ";" << '\n';
    os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << plaintext_id << ");"
       << '\n';
  }

  template <typename T>
  void write_vector_encoding(
    std::ostream &os, const std::string &plaintext_id, const std::vector<T> &vector_value,
    const std::string &vector_type) const
  {
    // dump the vector
    size_t vector_size = vector_value.size();
    std::string vector_value_str("{");
    for (size_t i = 0; i < vector_size; i++)
    {
      vector_value_str += std::to_string(vector_value[i]);
      if (i < vector_size - 1)
        vector_value_str += ",";
    }
    vector_value_str += "}";
    const std::string vector_id = plaintext_id + "_clear";
    os << "vector<" << vector_type << ">"
       << " " << vector_id << " = " << vector_value_str << ";" << '\n';

    // encoding
    os << types_map[ir::plaintextType] << " " << plaintext_id << ";" << '\n';
    os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << plaintext_id << ");"
       << '\n';
  }

  bool is_initialized() const { return this->is_init; }
};

struct ContextWriter
{
private:
  params_selector::EncryptionParameters *params;
  fhecompiler::Scheme scheme_type;

  const std::vector<std::string> scheme_type_str = {"bfv", "bgv", "ckks"};
  const std::vector<std::string> security_level_str = {
    "sec_level_type::tc128", "sec_level_type::tc192", "sec_level_type::tc256"};

public:
  bool is_defined = false;

  ContextWriter(params_selector::EncryptionParameters *_params, fhecompiler::Scheme scheme_t)
    : params(_params), scheme_type(scheme_t)
  {}

  void write_plaintext_modulus(std::ostream &os)
  {
    os << params_identifier_literal << "." << set_plain_modulus_intruction << "(" << params->plaintext_modulus << ");"
       << '\n';
  }

  void write_plaintext_modulus_bit_length(std::ostream &os)
  {
    if (params->poly_modulus_degree == 0)
      throw("invalid plynomial modulus degree in write_plaintext_modulus_bit_length \n");

    os << params_identifier_literal << "." << set_plain_modulus_intruction << "(PlainModulus::Batching("
       << params->poly_modulus_degree << "," << params->plaintext_modulus_bit_length << "));" << '\n';
  }

  void write_user_defined_coefficient_modulus(std::ostream &os)
  {
    if (!params->coef_modulus.empty())
    {
      std::string coef_modulus_identifier = "coef_mod";
      size_t coef_mod_size = params->coef_modulus.size();
      os << "std::vector<int> " << coef_modulus_identifier << " = ";
      os << "{";
      for (size_t i = 0; i < params->coef_modulus.size(); i++)
      {
        os << params->coef_modulus[i];
        if (i < params->coef_modulus.size() - 1)
        {
          os << ", ";
        }
      }
      os << "};" << '\n';
      os << params_identifier_literal << "." << set_coef_modulus_instruction << "(CoeffModulus::Create("
         << params->poly_modulus_degree << "," << coef_modulus_identifier << "));" << '\n';
    }
    else
      throw("coef moulus is not set by the user\n");
  }

  void write_default_coefficient_modulus_BFV(std::ostream &os)
  {
    if (scheme_type != fhecompiler::Scheme::bfv)
    {
      throw("default coef modulus is supported only for BFV\n");
    }

    os << params_identifier_literal << "." << set_coef_modulus_instruction << "(CoeffModulus::BFVDefault("
       << params->poly_modulus_degree << "," << security_level_str[static_cast<int>(params->security_level)] << "));"
       << '\n';
  }

  void write_polynomial_modulus_degree(std::ostream &os)
  {
    os << params_identifier_literal << "." << set_poly_modulus_degree_instruction << "(" << params->poly_modulus_degree
       << ");" << '\n';
  }

  void write_parameters(std::ostream &os)
  {

    os << params_type_literal << " " << params_identifier_literal
       << "(scheme_type::" << scheme_type_str[static_cast<int>(scheme_type)] << ");" << '\n';

    write_polynomial_modulus_degree(os);
    if (!params->coef_modulus.empty())
    {
      write_user_defined_coefficient_modulus(os);
    }
    else
      write_default_coefficient_modulus_BFV(os);

    if (params->plaintext_modulus_bit_length == 0)
    {
      write_plaintext_modulus(os);
    }
    else
      write_plaintext_modulus_bit_length(os);
  }

  /**/

  /*
    a + b + c*k*z + .... +
  */

  void write_context(std::ostream &os)
  {
    os << context_type_literal << " " << context_function_name << "()"
       << "{" << '\n';
    write_parameters(os);
    os << context_type_literal << " " << context_identifier << "(" << params_identifier_literal << ");" << '\n';
    os << "return " << context_identifier << ";" << '\n';
    os << "}" << '\n';
  };
};

inline std::string stringfy_string(const std::string &str)
{
  return '"' + str + '"';
}

} // namespace translator
