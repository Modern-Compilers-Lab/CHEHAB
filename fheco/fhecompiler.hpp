#pragma once

#include "ciphertext.hpp"
#include "fhecompiler_const.hpp"
#include "ops_overloads.hpp"
#include "plaintext.hpp"
#include "program.hpp"
#include "scalar.hpp"
#include "translator.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace fhecompiler
{

/*program init, scale is only for ckks*/
void init(
  const std::string &name, int bit_width, bool signedness = true, std::size_t vec_size = 1024,
  SecurityLevel sec_level = SecurityLevel::tc128, Scheme scheme = Scheme::bfv, double scale = 0.0);

// paramters selection manually

// compile
void compile(const std::string &output_filename);

}; // namespace fhecompiler
