#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "seal/seal.h"

void cryptonets_147457_noopt(const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
std::unordered_map<std::string, seal::Plaintext> &encoded_outputs,
const seal::BatchEncoder &encoder,
const seal::Encryptor &encryptor,
const seal::Evaluator &evaluator,
const seal::RelinKeys &relin_keys,
const seal::GaloisKeys &galois_keys);

std::vector<int> get_rotation_steps_cryptonets_147457_noopt();
