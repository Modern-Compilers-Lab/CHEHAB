#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("gx_kernel", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext c0("c0", fhecompiler::VarType::input);

    fhecompiler::Ciphertext c1 = c0 << 1;
    fhecompiler::Ciphertext c2 = c0 << 5;
    fhecompiler::Ciphertext c3 = c0 << 6;
    fhecompiler::Ciphertext c4 = c0 << -1;
    fhecompiler::Ciphertext c5 = c0 << -4;
    fhecompiler::Ciphertext c6 = c0 << -6;
    fhecompiler::Ciphertext c7 = c1 + c2;
    fhecompiler::Ciphertext c8 = c3 + c4;
    fhecompiler::Ciphertext c9 = c5 + c6;
    fhecompiler::Ciphertext c10 = c7 + c8;
    fhecompiler::Ciphertext c11 = c9 + c9;
    fhecompiler::Ciphertext c12 = c10 + c11;
    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    output = c1 + c2 + c3 + c4 + c5 + c6;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("gx_kernel.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << '\n';
  }

  return 0;
}
