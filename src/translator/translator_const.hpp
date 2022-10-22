#pragma once

#include "ir_const.hpp"
#include <unordered_map>

namespace translator
{

/* return types map, it maps IR types with corresponding literal for code generation that targets the API */
std::unordered_map<ir::TermType, const char *> types_map = {

  {ir::ciphertextType, "Ciphertext "}, {ir::plaintextType, "Plaintext"}

};

const char *scalar_int = "uint64_t";
const char *scalar_float = "double";

/* ops_map maps and IR operation code with corresponding literal for code generation that targets the API*/
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

/* general literals */
const char *new_line = "\n";
const char *end_of_command = ";";
const char *open_bracket = "[";
const char *close_bracket = "]";
const char *open_curly_bracket = "{";
const char *close_curly_bracket = "}";
const char *open_parantesis = "(";
const char *close_parantesis = ")";
const char *return_stmnt = "return ";
const char *vector_literal = "vector";

/* literals that relates to FHE */

const char *encode_literal = "Encode";
const char *decode_literal = "Decode";
const char *encrypt_literal = "Encrypt";
const char *decrypt_literal = "Decrypt";
const char *context_literal = "Context";
const char *public_key_literal = "PublicKey";
const char *secret_key_literal = "SecretKey";

} // namespace translator
