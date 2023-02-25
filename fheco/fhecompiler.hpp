#pragma once

#include "ciphertext.hpp"
#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "ops_overloads.hpp"
#include "params_selector.hpp"
#include "plaintext.hpp"
#include "program.hpp"
#include "scalar.hpp"
#include "translator.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#define DEFAULT_VECTOR_SIZE 1024

namespace fhecompiler
{

/*program init, scale is only for ckks*/
void init(
  const std::string &program_name, Scheme program_scheme, Backend backend = Backend::SEAL,
  size_t vector_size = DEFAULT_VECTOR_SIZE, double scale = 0.0);

// paramters selection manually

// compile
void compile(const std::string &output_filename, params_selector::EncryptionParameters *params);

}; // namespace fhecompiler
