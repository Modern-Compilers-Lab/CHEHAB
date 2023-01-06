#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

inline fhecompiler::Ciphertext sum_all_slots(const fhecompiler::Ciphertext &x)
{
  // number of slots needs to be know by the user
  // we assume here that we have 5 slots in x
  fhecompiler::Ciphertext result("sum_result", fhecompiler::VarType::temp);
  result = x;
  fhecompiler::Ciphertext x1 = x << 1;
  result += x1;
  fhecompiler::Ciphertext x2 = x1 << 1;
  result += x2;
  fhecompiler::Ciphertext x3 = x2 << 1;
  result += x3;
  fhecompiler::Ciphertext x4 = x3 << 1;
  result += x4;
  // result of sum will be in the first slot
  return result;
}

int test21_frontend()
{
  try
  {
    fhecompiler::init("test21", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext slots_sum("slots_sum", fhecompiler::VarType::output);
    fhecompiler::Ciphertext dummy_output("ct_otuput", fhecompiler::VarType::output);

    slots_sum = sum_all_slots(ct1);

    dummy_output = -ct1;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test21.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
