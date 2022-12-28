#pragma once

#include "ciphertext.hpp"
#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "ops_overloads.hpp"
#include "optimizer.hpp"
#include "params_selector.hpp"
#include "plaintext.hpp"
#include "program.hpp"
#include "scalar.hpp"
#include "translator.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace fhecompiler
{

// init program
void init(
  const std::string &program_name, size_t plaintext_modulus, size_t number_of_slots, Scheme program_scheme,
  Backend backend = Backend::SEAL, double scale = 0.0);

// paramters selection manually

// compile
void compile(const std::string &output_filename, params_selector::EncryptionParameters *params);

}; // namespace fhecompiler
