#pragma once

#include"plaintext.hpp"
#include"ciphertext.hpp"
#include"scalar.hpp"
#include"program.hpp"
#include"fhecompiler_const.hpp"
#include"ops_overloads.hpp"
#include<iostream>
#include<string>
#include<memory>

ir::Program* program;

namespace fhecompiler
{

void init(const std::string& program_name, size_t dim)
{
  static ir::Program program_object(program_name, dim);
  program = &program_object;
}

void set_program_scheme(Scheme program_scheme )
{ 
  program->set_scheme(program_scheme); 
}

void compile() 
{
  program->traverse_dataflow();
}

} // namespace fhecompiler

