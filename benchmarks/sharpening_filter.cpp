#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("sharpening_filter", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext img("img", fhecompiler::VarType::input);

    int n = 512;

    fhecompiler::Ciphertext r0 = img * -8;
    fhecompiler::Ciphertext r1 = img << -n - 1;
    fhecompiler::Ciphertext r2 = img << -n;
    fhecompiler::Ciphertext r3 = img << -n + 1;
    fhecompiler::Ciphertext r4 = img << -1;
    fhecompiler::Ciphertext r5 = img << 1;
    fhecompiler::Ciphertext r6 = img << n - 1; // 511
    fhecompiler::Ciphertext r7 = img << n; // 512
    fhecompiler::Ciphertext r8 = img << n + 1; // 513
    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    fhecompiler::Ciphertext output2("output2", fhecompiler::VarType::output);
    /*
    y' = (x << 1)
    y = y' + x
    1 + |(n-1) + (n) + (n+1)|
    1 + |(n-1) + (n) + (n+1)|
    (y << n) + y' + (x << (n-1))
    (y << n) + (x << (n-1)) + y'
      fix a base r = 2
      //first thing is that steps are sorted, with length is 7
      log2(n)
      [1, 2, 3, 4, 5, 6, 7, 8, 9]
    */
    /*
     [1, 4, 5, 9, 13]
    */

    /*
      sliding window
    */

    /*
        y' = x << 1
        y = y' + x
        (x << (n-1)) + (x << n) + (x << (n+1))
        (y << n) + (x)
    */

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
