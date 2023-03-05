#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("sharpening_filter", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext img("img", fhecompiler::VarType::input);

    int n = 720;

    fhecompiler::Ciphertext r0 = img * -8;
    fhecompiler::Ciphertext r1 = img << -n - 1;
    fhecompiler::Ciphertext r2 = img << -n;
    fhecompiler::Ciphertext r3 = img << -n + 1;
    fhecompiler::Ciphertext r4 = img << -1;
    fhecompiler::Ciphertext r5 = img << 1;
    fhecompiler::Ciphertext r6 = img << n - 1;
    fhecompiler::Ciphertext r7 = img << n;
    fhecompiler::Ciphertext r8 = img << n + 1;
    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    fhecompiler::Ciphertext output2("output2", fhecompiler::VarType::output);

    output = 2 * img - (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("sharpening_filter.hpp", &params);
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
