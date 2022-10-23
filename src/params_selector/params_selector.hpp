#pragma once

#include "encryption_context.hpp"
#include "program.hpp"
#include <vector>

namespace params_selector
{

class ParameterSelector
{
private:
  ir::Program *program;

public:
  ParameterSelector(ir::Program *prgm) : program(prgm) {}
  EncryptionContext select_parameters();
};
} // namespace params_selector
