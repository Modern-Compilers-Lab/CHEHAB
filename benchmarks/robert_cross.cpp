#include "fhecompiler/fhecompiler.hpp"

using namespace fhecompiler;

int main()
{

  try
  {
    fhecompiler::init("box_blur", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext img("img", fhecompiler::VarType::input);

    int width = 32;

    Ciphertext bottom_row = img << width;
    // Gx
    Ciphertext gx = img - (bottom_row << 1);
    // Gy
    Ciphertext gy = (img << 1) - bottom_row;
    // combine
    Ciphertext result = fhecompiler::square(gx) + fhecompiler::square(gy);
    Ciphertext output("output", VarType::output);
    output = result;

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
