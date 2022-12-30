#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

int test2_frontend()
{

  try
  {

    fhecompiler::init("test2", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Plaintext pt1("pt1", fhecompiler::VarType::constant);

    pt1 = std::vector<int64_t>({1, 2, 3, 4, 5});

    fhecompiler::Ciphertext add_cipher_plain("add_cipher_plain", fhecompiler::VarType::output);

    add_cipher_plain = ct1 + pt1;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test2.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
