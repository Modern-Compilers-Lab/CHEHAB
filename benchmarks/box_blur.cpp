#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("box_blur", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext c0("c0", fhecompiler::VarType::input);

    fhecompiler::Ciphertext c1 = c0 << 1;
    fhecompiler::Ciphertext c2 = c0 << 5;
    fhecompiler::Ciphertext c3 = c0 << 6;
    fhecompiler::Ciphertext c4 = c1 + c0;
    fhecompiler::Ciphertext c5 = c2 + c3;
    fhecompiler::Ciphertext c6 = c4 + c5;
    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    output = c6;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("box_blur.hpp", &params);
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
