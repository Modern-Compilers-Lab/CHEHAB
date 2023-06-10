#pragma once

#include "fheco/code_gen/common.hpp"
#include "fheco/ir/term.hpp"
#include <cstddef>
#include <string_view>
#include <unordered_map>

using namespace std::literals;

namespace fheco::code_gen
{
inline constexpr std::string_view seal_namespace{"seal"};

inline constexpr std::string_view cipher_type{"Ciphertext"};
inline constexpr std::string_view plain_type{"Plaintext"};

inline constexpr std::string_view header_encrypted_io_type{"std::unordered_map<std::string, seal::Ciphertext>"};
inline constexpr std::string_view header_encoded_io_type{"std::unordered_map<std::string, seal::Plaintext>"};

inline constexpr std::string_view source_encrypted_io_type{"unordered_map<string, Ciphertext>"};
inline constexpr std::string_view source_encoded_io_type{"unordered_map<string, Plaintext>"};

inline constexpr std::string_view encrypted_inputs_container_id{"encrypted_inputs"};
inline constexpr std::string_view encoded_inputs_container_id{"encoded_inputs"};

inline constexpr std::string_view encrypted_outputs_container_id{"encrypted_outputs"};
inline constexpr std::string_view encoded_outputs_container_id{"encoded_outputs"};

inline constexpr std::string_view relin_keys_type{"RelinKeys"};
inline constexpr std::string_view relin_keys_id{"relin_keys"sv};

inline constexpr std::string_view galois_keys_type{"GaloisKeys"};
inline constexpr std::string_view galois_keys_id{"galois_keys"};

inline const std::string rotation_steps_getter_id = "get_rotation_steps";

inline constexpr std::size_t line_threshold = 16;

inline constexpr std::string_view encoder_type{"BatchEncoder"};
inline constexpr std::string_view encoder_id{"encoder"};
inline constexpr std::string_view slot_count_id{"slot_count"};
inline constexpr std::string_view encode{"encode"};
inline constexpr std::string_view decode{"decode"};

inline constexpr std::string_view encryptor_type = "Encryptor"sv;
inline constexpr std::string_view encryptor_id{"encryptor"};
inline constexpr std::string_view encrypt{"encrypt"};
inline constexpr std::string_view signed_slot_type{"std::int64_t"};
inline constexpr std::string_view unsigned_slot_type{"std::uint64_t"};

inline constexpr std::string_view evaluator_type{"Evaluator"};
inline constexpr std::string_view evaluator_id{"evaluator"};

inline constexpr std::string_view header_includes{
  R"(#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "seal/seal.h"
)"};

inline constexpr std::string_view source_includes{
  R"(#include <cstddef>
#include <cstdint>
#include <utility>
)"};

inline constexpr std::string_view source_usings{
  R"(using namespace std;
using namespace seal;
)"};

inline const std::unordered_map<OpType, std::string_view, HashOpType, EqualOpType> operation_mapping = {
  {{ir::OpCode::Type::encrypt, {ir::Term::Type::plain}}, "encrypt"sv},
  {{ir::OpCode::Type::add, {ir::Term::Type::cipher, ir::Term::Type::cipher}}, "add"sv},
  {{ir::OpCode::Type::add, {ir::Term::Type::cipher, ir::Term::Type::plain}}, "add_plain"sv},
  {{ir::OpCode::Type::sub, {ir::Term::Type::cipher, ir::Term::Type::cipher}}, "sub"sv},
  {{ir::OpCode::Type::sub, {ir::Term::Type::cipher, ir::Term::Type::plain}}, "sub_plain"sv},
  {{ir::OpCode::Type::negate, {ir::Term::Type::cipher}}, "negate"sv},
  {{ir::OpCode::Type::rotate, {ir::Term::Type::cipher}}, "rotate_rows"sv},
  {{ir::OpCode::Type::square, {ir::Term::Type::cipher}}, "square"sv},
  {{ir::OpCode::Type::mul, {ir::Term::Type::cipher, ir::Term::Type::cipher}}, "multiply"sv},
  {{ir::OpCode::Type::mul, {ir::Term::Type::cipher, ir::Term::Type::plain}}, "multiply_plain"sv},
  {{ir::OpCode::Type::mod_switch, {ir::Term::Type::cipher}}, "mod_switch_to_next"sv},
  {{ir::OpCode::Type::relin, {ir::Term::Type::cipher}}, "relinearize"sv},
};
} // namespace fheco::code_gen
