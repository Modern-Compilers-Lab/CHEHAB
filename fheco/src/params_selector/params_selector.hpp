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
  EncryptionParameters encryption_parameters;
  void check_nb_slots_with_vector_size(const EncryptionParameters &params);

public:
  ParameterSelector(ir::Program *prgm) : program(prgm) {}
  EncryptionParameters select_parameters();
  void fix_parameters(EncryptionParameters &params);
  void check_parameters(EncryptionParameters &params);
};
} // namespace params_selector
