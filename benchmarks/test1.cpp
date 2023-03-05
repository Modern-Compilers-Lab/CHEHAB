#include "fhecompiler/fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

int main()
{
  try
  {

    fhecompiler::init("test1", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext A("A", fhecompiler::VarType::input);
    fhecompiler::Ciphertext B("B", fhecompiler::VarType::input);
    fhecompiler::Ciphertext C("C", fhecompiler::VarType::input);
    fhecompiler::Ciphertext D("D", fhecompiler::VarType::input);
    fhecompiler::Ciphertext E("E", fhecompiler::VarType::input);

    fhecompiler::Ciphertext output1("output1", VarType::output);
    fhecompiler::Ciphertext output2("output2", VarType::output);
    fhecompiler::Ciphertext output3("output3", VarType::output);
    fhecompiler::Ciphertext output4("output4", VarType::output);
    fhecompiler::Ciphertext output5("output5", VarType::output);

    /*
    output1 = B << 3;

    for (int32_t i = 0; i < 32; i++)
      output1 += (A << (i + 1));
    */

    fhecompiler::Plaintext pt1(std::vector<int64_t>({1, 2, 4, 5, 6}));

    fhecompiler::Plaintext pt2(std::vector<int64_t>({10, 22, 34, 45, 56}));

    fhecompiler::Plaintext pt_input1("pt_input1", fhecompiler::VarType::input);
    fhecompiler::Plaintext pt_input2("pt_input2", fhecompiler::VarType::input);

    fhecompiler::Plaintext pt_output("pt_output", fhecompiler::VarType::output);

    fhecompiler::Plaintext pt_taged_tmp("pt_const", fhecompiler::VarType::temp);

    fhecompiler::Scalar n = 3;

    fhecompiler::Scalar s(12);

    fhecompiler::Plaintext pt_tmp = -pt1 + pt1 + (100 * s - pt2); // + pt2 + (23 + s);

    pt_output = pt_tmp;
    // pt_taged_tmp = pt_input1 + pt_input2;

    pt_output += 888;

    pt_output += pt_output;

    pt_input1 += pt_input2;

    // pt_output = pt_input1 + pt_input2; //(A - (B + C)) + C;

    pt_output += pt_output;

    // fhecompiler::Plaintext pt_one(std::vector<int64_t>({1, 1, 1}));

    output1 = (0 - A) * pt_input1;

    output1 = (A + (5 - B)) + pt_output + pt_input1 + D + C + 1337; // * pt2 + (pt1 + pt2));

    // output2 = (((A + pt_output) + pt_output) * pt_output) * (pt_output * 23);

    std::vector<int64_t> zero_vector;
    for (size_t i = 0; i < 124; i++)
      zero_vector.push_back(0);

    fhecompiler::Ciphertext tmp = A * 0;

    output1 = B + A * 1;

    output2 = B + A + (A * A) + (pt_input1 * pt_input1);

    pt_output = pt_input1 * 0;

    A = B;

    output3 = A * zero_vector;

    output4 = C + B + exponentiate(D, 5) + (A << 5);

    output5 = (E * (B * (C * D)));

    params_selector::EncryptionParameters params;
    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("test1.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }
  return 0;
}
