#pragma once

#include "ir_const.hpp"
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
  {ir::TermType::scalarType, "inputs_scalar"},
  {ir::TermType::plaintextType, "plain_inputs"},
  {ir::TermType::ciphertextType, "cipher_inputs"}};

/* literals related to api/backend */
const char *scalar_int = "uint64_t";
const char *scalar_float = "double";
const char *encode_literal = "Encode";
const char *decode_literal = "Decode";
const char *encrypt_literal = "Encrypt";
const char *decrypt_literal = "Decrypt";
const char *context_literal = "Context";
const char *public_key_literal = "PublicKey";
const char *secret_key_literal = "SecretKey";

/* general literals for C++ */
const char *new_line = "\n";
const char *end_of_command = ";";
const char *open_bracket = "[";
const char *close_bracket = "]";
const char *open_parantesis = "(";
const char *close_parantesis = ")";
const char *return_stmnt = "return ";
const char *vector_literal = "vector";
const char *assign_literal = "=";
const char *start_block = "{";
const char *end_block = "}";

} // namespace translator
