#pragma once

#include"plaintext.hpp"
#include"ciphertext.hpp"
#include"scalar.hpp"
#include"program.hpp"
#include"fhecompiler_const.hpp"
#include<string>
#include<memory>

std::shared_ptr<ir::Program> program;

namespace fhecompiler
{

void init(const std::string& program_name) { program = std::make_shared<ir::Program> (program_name); }
void set_program_scheme(Scheme program_scheme ) { program->set_scheme(program_scheme); }

} // namespace fhecompiler

