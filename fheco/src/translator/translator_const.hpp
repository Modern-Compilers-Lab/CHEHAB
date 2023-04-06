#pragma once

#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include <cstddef>
#include <fstream>
#include <optional>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#define INLINE inline

namespace translator
{
/* return types map, it maps IR types with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::TermType, std::string> types_map = {

  {ir::TermType::ciphertext, "seal::Ciphertext"}, {ir::TermType::plaintext, "seal::Plaintext"}

};

/* ops_map maps IR operation code with corresponding literal for code generation that targets the API */
INLINE std::unordered_map<ir::OpCode, std::string> ops_map = {

  {ir::OpCode::encrypt, "encrypt"},
  {ir::OpCode::add, "add"},
  {ir::OpCode::add_plain, "add_plain"},
  {ir::OpCode::assign, "="},
  {ir::OpCode::exponentiate, "exponentiate"},
  {ir::OpCode::negate, "negate"},
  {ir::OpCode::modswitch, "mod_switch_to_next"},
  {ir::OpCode::mul, "multiply"},
  {ir::OpCode::mul_plain, "multiply_plain"},
  {ir::OpCode::sub, "sub"},
  {ir::OpCode::sub_plain, "sub_plain"},
  {ir::OpCode::rescale, "rescale"},
  {ir::OpCode::square, "square"},
  {ir::OpCode::rotate, "rotate_rows"},
  {ir::OpCode::rotate_rows, "rotate_rows"},
  {ir::OpCode::rotate_columns, "rotate_columns"},
  {ir::OpCode::relinearize, "relinearize"}

};

INLINE std::unordered_map<ir::OpCode, std::string> ops_map_inplace = {

  {ir::OpCode::encrypt, "encrypt"},
  {ir::OpCode::add, "add_inplace"},
  {ir::OpCode::add_plain, "add_plain_inplace"},
  {ir::OpCode::assign, "="},
  {ir::OpCode::exponentiate, "exponentiate_inplace"},
  {ir::OpCode::negate, "negate_inplace"},
  {ir::OpCode::modswitch, "mod_switch_to_next_inplace"},
  {ir::OpCode::mul, "multiply_inplace"},
  {ir::OpCode::mul_plain, "multiply_plain_inplace"},
  {ir::OpCode::sub, "sub_inplace"},
  {ir::OpCode::sub_plain, "sub_plain_inplace"},
  {ir::OpCode::rescale, "rescale_inplace"},
  {ir::OpCode::square, "square_inplace"},
  {ir::OpCode::rotate, "rotate_rows_inplace"},
  {ir::OpCode::rotate_rows, "rotate_rows_inplace"},
  {ir::OpCode::rotate_columns, "rotate_columns_inplace"},
  {ir::OpCode::relinearize, "relinearize_inplace"}

};

INLINE std::unordered_set<ir::OpCode> inplace_instructions = {
  ir::OpCode::add,          ir::OpCode::add_plain, ir::OpCode::sub,
  ir::OpCode::sub_plain,    ir::OpCode::mul,       ir::OpCode::mul_plain,
  ir::OpCode::negate,       ir::OpCode::rotate,    ir::OpCode::rotate_columns,
  ir::OpCode::rotate_rows,  ir::OpCode::modswitch, ir::OpCode::relinearize,
  ir::OpCode::exponentiate, ir::OpCode::square};

/* literals related to api/backend */
INLINE std::string params_type_literal = "seal::EncryptionParameters";
INLINE std::string params_identifier_literal = "params";
INLINE std::string scalar_int = "std::int64_t";
INLINE std::string scalar_uint = "std::uint64_t";
INLINE std::string scalar_float = "double";
INLINE std::string encode_literal = "encode";
INLINE std::string decode_literal = "decode";
INLINE std::string encrypt_literal = "encrypt";
INLINE std::string decrypt_literal = "decrypt";
INLINE std::string context_type_literal = "seal::SEALContext";
INLINE std::string context_identifier = "context";
INLINE std::string public_key_literal = "seal::PublicKey";
INLINE std::string public_key_identifier = "public_key";
INLINE std::string secret_key_literal = "seal::SecretKey";
INLINE std::string secret_key_identifier = "secret_key";
INLINE std::string relin_keys_type_literal = "seal::RelinKeys";
INLINE std::string relin_keys_identifier = "relin_keys";
INLINE std::string galois_keys_type_literal = "seal::GaloisKeys";
INLINE std::string galois_keys_identifier = "galois_keys";
INLINE std::string key_generator_type_literal = "seal::KeyGenerator";
INLINE std::string key_generator_identifier = "key_gen";
INLINE std::string evaluator_type_literal = "seal::Evaluator";
INLINE std::string evaluator_identifier = "evaluator";
INLINE std::string bv_encoder_type_literal = "seal::BatchEncoder";
INLINE std::string ckks_encoder_type_literal = "seal::CKKSEncoder";
INLINE std::string encoder_type_identifier = "encoder";
INLINE std::string encryptor_type_literal = "seal::Encryptor";
INLINE std::string encryptor_type_identifier = "encryptor";
INLINE std::string insert_object_instruction = "insert"; // instruction to insert inputs/outputs
INLINE std::string context_function_name = "create_context";
INLINE std::string set_plain_modulus_intruction = "set_plain_modulus";
INLINE std::string create_plain_modulus_intruction = "seal::PlainModulus::Batching";
INLINE std::string set_coeff_modulus_instruction = "set_coeff_modulus";
INLINE std::string create_coeff_modulus_instruction = "seal::CoeffModulus::Create";
INLINE std::string set_poly_modulus_degree_instruction = "set_poly_modulus_degree";
INLINE std::string encrypted_inputs_class_literal = "std::map<std::string, seal::Ciphertext>";
INLINE std::string encoded_inputs_class_literal = "std::map<std::string, seal::Plaintext>";
INLINE std::string encoded_outputs_class_literal = "std::map<std::string, seal::Plaintext>";
INLINE std::string encrypted_outputs_class_literal = "std::map<std::string, seal::Ciphertext>";
INLINE std::string headers_include =
  "#include \"seal/seal.h\"\n#include <vector>\n#include <map>\n#include <cstdint>\n";
INLINE std::string rotation_step_type_literal = "int";
INLINE std::string gen_steps_function_signature = "std::vector<int> get_rotations_steps()";

INLINE std::unordered_map<ir::TermType, std::string> outputs_class_identifier = {
  {ir::TermType::plaintext, "encoded_outputs"}, {ir::TermType::ciphertext, "encrypted_outputs"}};

INLINE std::unordered_map<ir::TermType, std::string> inputs_class_identifier = {
  {ir::TermType::plaintext, "encoded_inputs"}, {ir::TermType::ciphertext, "encrypted_inputs"}};

INLINE std::unordered_map<ir::OpCode, std::string> get_other_args_by_opcode = {
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

  Argument(std::string type, std::string identifier, AccessType _access_type)
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
       << public_key_identifier << ");\n";
  }

  void write_encryption(std::ostream &os, const std::string &plaintext_id, const std::string &destination_cipher) const
  {
    os << types_map[ir::TermType::ciphertext] << " " << destination_cipher << ";\n";
    os << encryptor_identifier << "." << encrypt_instruction_literal << "(" << plaintext_id << "," << destination_cipher
       << ");\n";
  }

  bool is_initialized() const { return this->is_init; }
};

struct EvaluationWriter
{
private:
  std::string evaluator_identifier;
  std::string context_identifier;
  std::string evaluator_type_literal;

public:
  EvaluationWriter() = default;

  EvaluationWriter(const std::string &evaluator_type, const std::string &evaluator_id, const std::string &context_id)
    : evaluator_identifier(evaluator_id), context_identifier(context_id), evaluator_type_literal(evaluator_type)
  {}

  ~EvaluationWriter() {}

  void write_unary_operation(
    std::ostream &os, ir::OpCode opcode, const std::string &destination_id, const std::string &lhs_id,
    ir::TermType type)
  {

    // if destination_id == lhs_id then the instruction is inplace

    bool is_inplace = destination_id == lhs_id;

    std::string other_args("");
    auto it = get_other_args_by_opcode.find(opcode);

    std::string instruction_code = (is_inplace ? ops_map_inplace[opcode] : ops_map[opcode]);

    if (it != get_other_args_by_opcode.end())
    {
      other_args = it->second;
    }

    if (!is_inplace)
      os << types_map[type] << " " << destination_id << ";\n";

    if (other_args.length() == 0)
    {
      os << evaluator_identifier << "." << instruction_code << "(" << lhs_id;
      if (!is_inplace)
      {
        os << "," << destination_id << ");";
      }
      else
        os << ");";
    }
    else
    {
      os << evaluator_identifier << "." << instruction_code << "(" << lhs_id << "," << other_args;
      if (!is_inplace)
        os << "," << destination_id << ");";
      else
        os << ");";
    }
    os << '\n';
  }

  void write_binary_operation(
    std::ostream &os, ir::OpCode opcode, const std::string &destination_id, const std::string &lhs_id,
    const std::string &rhs_id, ir::TermType type)
  {
    // if destination_id == lhs_id then it is an inplace instruction

    bool is_inplace = destination_id == lhs_id;

    if (!is_inplace)
      os << types_map[type] << " " << destination_id << ";\n";

    std::string instruction_code = (is_inplace ? ops_map_inplace[opcode] : ops_map[opcode]);
    std::string other_args("");
    auto it = get_other_args_by_opcode.find(opcode);

    if (it != get_other_args_by_opcode.end())
    {
      other_args = it->second;
    }
    os << evaluator_identifier << "." << instruction_code << "(" << lhs_id << "," << rhs_id;
    if (other_args.length() > 0)
    {
      os << ", " << other_args;
    }
    if (!is_inplace)
      os << "," << destination_id << ");";
    else
      os << ");";
    os << '\n';
  }
};

struct EncodingWriter
{
private:
  std::size_t nb_slots;
  std::string encoder_identifier;
  std::string encoder_instruction_literal;
  bool is_init = false;

public:
  EncodingWriter() = default;

  EncodingWriter(std::size_t nb_slots, const std::string &encoder_id, const std::string &encode_inst_literal)
    : nb_slots(nb_slots), encoder_identifier(encoder_id), encoder_instruction_literal(encode_inst_literal)
  {}
  ~EncodingWriter() {}

  void write_scalar_encoding(
    std::ostream &os, const std::string &plaintext_id, const std::string &scalar_value, const std::string &scalar_type,
    double scale = 0.0) const
  {
    // create a vector
    const std::string vector_id = plaintext_id + "_clear";
    os << "std::vector<" << scalar_type << ">"
       << " " << vector_id << "(" << nb_slots << "," << scalar_value << ");\n";

    os << types_map[ir::TermType::plaintext] << " " << plaintext_id << ";\n";
    if (scale > 0.0)
    {
      os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << scale << ","
         << plaintext_id << ");\n";
    }
    else
    {
      os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << plaintext_id
         << ");\n";
    }
  }

  template <typename T>
  void write_vector_encoding(
    std::ostream &os, const std::string &plaintext_id, const std::vector<T> &vector_value,
    const std::string &vector_type, double scale = 0.0) const
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
    os << "std::vector<" << vector_type << ">"
       << " " << vector_id << " = " << vector_value_str << ";\n";

    // encoding
    os << types_map[ir::TermType::plaintext] << " " << plaintext_id << ";\n";
    if (scale > 0.0)
    {
      os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << scale << ","
         << plaintext_id << ");\n";
    }
    else
    {
      os << encoder_identifier << "." << encoder_instruction_literal << "(" << vector_id << "," << plaintext_id
         << ");\n";
    }
  }
};

struct ContextWriter
{
private:
  param_selector::EncryptionParameters params;
  fhecompiler::Scheme scheme_type;
  fhecompiler::SecurityLevel sec_level;
  bool uses_mod_switch;

  const std::vector<std::string> scheme_type_str = {"none", "bfv", "bgv", "ckks"};
  const std::vector<std::string> security_level_str = {"none", "tc128", "tc192", "tc256"};

public:
  bool is_defined = false;

  ContextWriter(
    const param_selector::EncryptionParameters &_params, fhecompiler::Scheme scheme_t, bool uses_mod_switch,
    fhecompiler::SecurityLevel sec_level)
    : params(_params), scheme_type(scheme_t), uses_mod_switch(uses_mod_switch), sec_level(sec_level)
  {}

  void write_plaintext_modulus(std::ostream &os)
  {
    os << params_identifier_literal << "." << set_plain_modulus_intruction << "(" << create_plain_modulus_intruction
       << "(" << params.poly_modulus_degree() << "," << params.plain_modulus_bit_size() << "));\n";
  }

  void write_coefficient_modulus(std::ostream &os)
  {
    os << params_identifier_literal << "." << set_coeff_modulus_instruction << "(" << create_coeff_modulus_instruction
       << "(" << params.poly_modulus_degree() << ",{";
    for (size_t i = 0; i < params.coeff_mod_bit_sizes().size(); i++)
    {
      os << params.coeff_mod_bit_sizes()[i];
      if (i < params.coeff_mod_bit_sizes().size() - 1)
        os << ", ";
    }
    os << "}));\n";
  }

  void write_polynomial_modulus_degree(std::ostream &os)
  {
    os << params_identifier_literal << "." << set_poly_modulus_degree_instruction << "(" << params.poly_modulus_degree()
       << ");\n";
  }

  void write_parameters(std::ostream &os)
  {

    os << params_type_literal << " " << params_identifier_literal
       << "(seal::scheme_type::" << scheme_type_str[static_cast<int>(scheme_type)] << ");\n";

    write_polynomial_modulus_degree(os);
    if (scheme_type != fhecompiler::Scheme::ckks)
      write_plaintext_modulus(os);
    write_coefficient_modulus(os);
  }

  void write_context(std::ostream &os)
  {
    os << context_type_literal << " " << context_function_name << "()"
       << "{\n";
    write_parameters(os);
    os << context_type_literal << " " << context_identifier << "(" << params_identifier_literal << ",";
    if (uses_mod_switch)
      os << "true";
    else
      os << "false";
    os << ","
       << "seal::sec_level_type::" << security_level_str[static_cast<int>(sec_level)] << ");\n";
    os << "return " << context_identifier << ";\n";
    os << "}\n";
  };
};

inline std::string stringfy_string(const std::string &str)
{
  return '"' + str + '"';
}

} // namespace translator
