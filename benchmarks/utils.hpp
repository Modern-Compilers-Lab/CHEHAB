#pragma once
#include "fhecompiler/params_selector.hpp"

namespace benchmarks_utils
{
void set_default_parameters(params_selector::EncryptionParameters *params)
{
  size_t polynomial_modulus_degree = 4096;
  size_t plaintext_modulus = 786433;

  params->set_plaintext_modulus(plaintext_modulus);
  params->set_polynomial_modulus_degree(polynomial_modulus_degree);
}

} // namespace benchmarks_utils
