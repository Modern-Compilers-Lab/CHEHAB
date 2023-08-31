#include "fhecompiler/fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

/*
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

fhecompiler::Plaintext pt1_const(std::vector<int64_t>({1, 3, 4}));
fhecompiler::Plaintext pt2_const(std::vector<int64_t>({23, 3, 4}));

auto x = fhecompiler::Ciphertext::encrypt(pt1_const);

auto y = fhecompiler::Ciphertext::encrypt(pt2_const);

auto z = x + y;

z += (x * x);

z *= z;

z *= pt1_const;

output1 = z;
*/

int main()
{
  try
  {

    fhecompiler::init("test1", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    Ciphertext ct1("a", VarType::input);
    Ciphertext ct2("b", VarType::input);
    Ciphertext ct3("c", VarType::input);
    /*
    vector<Ciphertext> ciphers_vector(10);
    for (int64_t i = 0; i < 10; i++)
    {
      ciphers_vector[i] = Ciphertext::encrypt(std::vector<int64_t>({i + 1}));
    }
    Ciphertext sum = Ciphertext::encrypt({0});
    std::cout << ciphers_vector[0].get_label() << " : label \n";
    for (size_t i = 0; i < ciphers_vector.size(); i++)
    {
      sum += ciphers_vector[i];
    }
    */
    /*
     std::vector<int64_t> val({1, 2, 3});
     // PackedVal val2({1, 6, 3});
     Plaintext pt(val);
     Plaintext pt_copy = pt;
     auto ct8 = (ct2 + pt_copy);
     pt_copy += 6;
     // Plaintext pt2(val2);
     auto ct4 = ct1 << 1;
     auto ct5 = ct1 << 2;
     auto ct6 = ct1 << 3;
     auto ct7 = ((ct1 + pt + std::vector<int64_t>({0}) + pt_copy) + pt_copy) - pt_copy;
     Ciphertext o1("o1", VarType::output);
     o1 = ct7;
     Ciphertext o2("o2", VarType::output);
     o2 = ct8;
     */
    Ciphertext ct_one = Ciphertext::encrypt(1);
    Ciphertext o3("o3", VarType::output);
    o3 = ct1 * ct_one;

    fhecompiler::compile();
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }
  return 0;
}
