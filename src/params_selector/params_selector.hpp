#pragma once

#include "encryption_parameters.hpp"
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
  EncryptionParameters select_parameters();
};
} // namespace params_selector
