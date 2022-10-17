#pragma once

#include "ciphertext.hpp"
#include "fhecompiler_const.hpp"
#include "ops_overloads.hpp"
#include "plaintext.hpp"
#include "program.hpp"
#include "scalar.hpp"
#include <iostream>
#include <memory>
#include <string>

ir::Program *program;

namespace fhecompiler
{

void init(const std::string &program_name, size_t dim, Scheme program_scheme)
{
  static ir::Program program_object(program_name, dim);
  program = &program_object;
  program->set_scheme(program_scheme);
}

void compile()
{
  program->traverse_dataflow();
}

} // namespace fhecompiler
